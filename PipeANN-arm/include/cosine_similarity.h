// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#ifdef _X86
#include <immintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#endif
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <limits>
#include <algorithm>
#include <stdexcept>

#include "fp16.h"

extern bool Avx2SupportedCPU;

namespace diskann {
  template<typename T>
  inline float compute_l2_norm(const T *vector, uint64_t ndims) {
    float norm = 0.0f;
    for (uint64_t i = 0; i < ndims; i++) {
      norm += (float) (vector[i] * vector[i]);
    }
    return std::sqrt(norm);
  }

  // FP16 specialization: convert each fp16 to fp32 before computing
  template<>
  inline float compute_l2_norm<uint16_t>(const uint16_t *vector, uint64_t ndims) {
    float norm = 0.0f;
    for (uint64_t i = 0; i < ndims; i++) {
      float val = fp16_ieee_to_fp32_value(vector[i]);
      norm += val * val;
    }
    return std::sqrt(norm);
  }

  template<typename T>
  inline float compute_cosine_similarity(const T *left, const T *right, uint64_t ndims) {
    float left_norm = compute_l2_norm<T>(left, ndims);
    float right_norm = compute_l2_norm<T>(right, ndims);
    float dot = 0.0f;
    for (uint64_t i = 0; i < ndims; i++) {
      dot += (float) (left[i] * right[i]);
    }
    float cos_sim = dot / (left_norm * right_norm);
    return cos_sim;
  }

  // FP16 specialization: convert each fp16 to fp32 before computing
  template<>
  inline float compute_cosine_similarity<uint16_t>(const uint16_t *left, const uint16_t *right, uint64_t ndims) {
    float left_norm = compute_l2_norm<uint16_t>(left, ndims);
    float right_norm = compute_l2_norm<uint16_t>(right, ndims);
    float dot = 0.0f;
    for (uint64_t i = 0; i < ndims; i++) {
      float l = fp16_ieee_to_fp32_value(left[i]);
      float r = fp16_ieee_to_fp32_value(right[i]);
      dot += l * r;
    }
    float cos_sim = dot / (left_norm * right_norm);
    return cos_sim;
  }

  inline std::vector<float> compute_cosine_similarity_batch(const float *query, const unsigned *indices,
                                                            const float *all_data, const unsigned ndims,
                                                            const unsigned npts) {
    std::vector<float> cos_dists;
    cos_dists.reserve(npts);

    for (size_t i = 0; i < npts; i++) {
      const float *point = all_data + (size_t) (indices[i]) * (size_t) (ndims);
      cos_dists.push_back(compute_cosine_similarity<float>(point, query, ndims));
    }
    return cos_dists;
  }
}  // namespace diskann
