# x86 platform CMake configuration
# This file is included when building for x86_64 platforms.

message(STATUS "Configuring for x86 platform")

# Check for AVX2 compiler support (AVX-512 is disabled)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)

# Check CPU runtime support for AVX2
set(CPU_SUPPORTS_AVX2 FALSE)
try_run(
	RUN_RESULT
	COMPILE_RESULT
	"${CMAKE_BINARY_DIR}"
	SOURCES "${PROJECT_SOURCE_DIR}/tests/test_cpu.cpp"
	COMPILE_OUTPUT_VARIABLE COMPILE_OUT
	RUN_OUTPUT_VARIABLE RUN_OUT
)

if(COMPILE_RESULT AND (RUN_RESULT EQUAL 1 OR RUN_RESULT EQUAL 3))
	set(CPU_SUPPORTS_AVX2 TRUE)
	message(STATUS "CPU supports AVX2")
else()
	message(STATUS "CPU does not support AVX2 or detection failed")
endif()

if(COMPILER_SUPPORTS_AVX2 AND CPU_SUPPORTS_AVX2)
	message(STATUS "Compiler and CPU supports -mavx2. Enabling AVX2.")
	add_definitions(-DUSE_AVX2)
else()
	message(STATUS "Compiler or CPU do not support -mavx2.")
endif()

# x86-specific compile flags
add_compile_options(-m64 -Wl,--no-as-needed)

# MKL Config
find_package(MKL)
link_libraries(mkl_intel_ilp64 mkl_intel_thread mkl_core iomp5 pthread m dl rt)
link_directories(${INTEL_ROOT}/lib/intel64 ${MKL_ROOT}/lib/intel64)

add_definitions(-DMKL_ILP64)
include_directories(include ${INTEL_ROOT}/include ${MKL_ROOT}/include)
