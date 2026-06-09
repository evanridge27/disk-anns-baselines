# ARM platform CMake configuration
# This file is included when building for ARM (aarch64) platforms.

message(STATUS "Configuring for ARM platform")

# ARM-specific compile flags
# -march=native lets the compiler auto-detect NEON capabilities
# -ftree-vectorize enables auto-vectorization
# No -m64 on ARM (it's an x86_64-only flag)
# No AVX/AVX2/AVX-512 on ARM
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -ftree-vectorize")

# BLAS library: OpenBLAS (ARM-compatible alternative to Intel MKL)
option(USE_KBLAS "Use KBLAS (Kunpeng Math Library) instead of OpenBLAS" OFF)

if(USE_KBLAS)
    message(STATUS "Using KBLAS linear algebra library")

    set(KML_BLAS_INCLUDE_DIR "/usr/local/kml/include")
    set(KML_BLAS_LIB_DIR "/usr/local/kml/lib/neon/kblas/omp")
    find_library(KML_BLAS kblas PATHS ${KML_BLAS_LIB_DIR})
    find_path(KML_BLAS_INCLUDE_DIR_2 kblas.h PATHS ${KML_BLAS_INCLUDE_DIR})

    if(KML_BLAS AND KML_BLAS_INCLUDE_DIR_2)
        message(STATUS "Found KML_BLAS: ${KML_BLAS}")
        include_directories(${KML_BLAS_INCLUDE_DIR_2})
        link_directories(${KML_BLAS_LIB_DIR})
    else()
        message(WARNING "KBLAS not found, falling back to OpenBLAS")
        set(USE_KBLAS OFF)
    endif()

    add_definitions(-DUSE_KBLAS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L /usr/local/kml/lib/neon/kblas/omp -lkblas")
endif()

if(NOT USE_KBLAS)
    message(STATUS "Using OpenBLAS linear algebra library")

    find_package(OpenBLAS QUIET)
    if(OpenBLAS_FOUND)
        message(STATUS "Found OpenBLAS via find_package: ${OpenBLAS_LIBRARIES}")
        include_directories(${OpenBLAS_INCLUDE_DIRS})
        link_libraries(${OpenBLAS_LIBRARIES})
    else()
        # Fallback: try to find OpenBLAS manually
        set(OpenBLAS_INCLUDE_DIR "/usr/include/openblas" CACHE PATH "OpenBLAS include directory")
        set(OpenBLAS_LIB_DIR "/usr/lib64" CACHE PATH "OpenBLAS library directory")

        find_library(OPENBLAS_LIB openblas PATHS ${OpenBLAS_LIB_DIR} /usr/lib /usr/lib/aarch64-linux-gnu)
        find_path(OPENBLAS_INCLUDE cblas.h PATHS ${OpenBLAS_INCLUDE_DIR} /usr/include /usr/include/openblas)

        if(OPENBLAS_LIB AND OPENBLAS_INCLUDE)
            message(STATUS "Found OpenBLAS: ${OPENBLAS_LIB}")
            message(STATUS "Found OpenBLAS include: ${OPENBLAS_INCLUDE}")
            include_directories(${OPENBLAS_INCLUDE})
            link_libraries(${OPENBLAS_LIB})
        else()
            message(FATAL_ERROR "OpenBLAS not found! Please install it: sudo apt install libopenblas-dev (Debian/Ubuntu) or sudo yum install openblas-devel (CentOS/RHEL)")
        endif()
    endif()
endif()

# No MKL_ILP64 on ARM - we use standard CBLAS interface
add_definitions(-DOPENBLAS_USE)
