// Check if CPU supports AVX512 and AVX2 instructions (x86)
// or NEON instructions (ARM)

#include <iostream>

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <cpuid.h>
#elif defined(__aarch64__) || defined(__arm__) || defined(_M_ARM64)
#include <sys/auxv.h>
#ifndef AT_HWCAP
#define AT_HWCAP 16
#endif
#ifndef AT_HWCAP2
#define AT_HWCAP2 26
#endif
#ifndef HWCAP_ASIMD
#define HWCAP_ASIMD (1 << 1)
#endif
#ifndef HWCAP2_SVE
#define HWCAP2_SVE (1 << 1)
#endif
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
int has_avx512() {
  int info[4];
#if defined(_MSC_VER)
  __cpuidex(info, 7, 0);
#else
  __cpuid_count(7, 0, info[0], info[1], info[2], info[3]);
#endif
  int support_avx512 = (info[1] & (1 << 16)) != 0;  // AVX512F bit
  int support_avx2 = (info[1] & (1 << 5)) != 0;     // AVX2 bit
  return support_avx512 << 1 | support_avx2;
}

int main() {
  return has_avx512();
}

#elif defined(__aarch64__) || defined(__arm__) || defined(_M_ARM64)
int has_neon() {
#if defined(__linux__)
  unsigned long hwcap = getauxval(AT_HWCAP);
  // NEON (ASIMD) is mandatory on AArch64, but check anyway
  int support_neon = (hwcap & HWCAP_ASIMD) != 0 ? 1 : 0;
  unsigned long hwcap2 = getauxval(AT_HWCAP2);
  int support_sve = (hwcap2 & HWCAP2_SVE) != 0 ? 1 : 0;
  // Return 3 for NEON+SVE (maps to "AVX512+AVX2" code path), 1 for NEON only
  return support_sve << 1 | support_neon;
#else
  // On non-Linux ARM, assume NEON is available (mandatory on AArch64)
  return 1;
#endif
}

int main() {
  return has_neon();
}

#else
// Unknown architecture - return 0 (no SIMD support detected)
int main() {
  return 0;
}
#endif
