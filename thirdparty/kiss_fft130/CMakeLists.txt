cmake_minimum_required (VERSION 3.3)

project ("KissFFT"
    LANGUAGES C)

include (GNUInstallDirs)

option (KISS_FFT_INSTALL_LIBS "Enable the install target")
option (KISS_FFT_USE_ALLOCA "Use alloca() instead of malloc()" ON)
option (KISS_FFT "Build FFT library" ON)
option (KISS_FASTCONV "Build fast convolution library" OFF)
option (KISS_FASTCONVR "" OFF)
option (KISS_PSDPNG "" OFF)
option (KISS_DUMPHDR "" OFF)
set (KISS_FFT_DATATYPE "double" CACHE STRING "The datatype to use for all transforms. Can be 'double', 'float', 'int32_t', 'int16_t', or 'simd'")
set (KISS_FFT_LIB_TYPE "STATIC" CACHE STRING "Library type: SHARED or STATIC")

set (KISS_FFT_USE_SIMD "0")
if (${KISS_FFT_DATATYPE} STREQUAL "int32_t")
    set (KISS_FFT_FIXED_POINT "32")
elseif (${KISS_FFT_DATATYPE} STREQUAL "int16_t")
    set (KISS_FFT_FIXED_POINT "16")
elseif (${KISS_FFT_DATATYPE} STREQUAL "simd")
    set (KISS_FFT_USE_SIMD "1")
elseif (${KISS_FFT_DATATYPE} STREQUAL "double" OR ${KISS_FFT_DATATYPE} STREQUAL "float")
else ()
    message (FATAL_ERROR "Unsupported datatype '${KISS_FFT_DATATYPE}'")
endif ()

configure_file ("include/kiss_fft/config.h.in" "include/kiss_fft/config.h")

###############################################################################
# source files and library definition
###############################################################################

if (KISS_FFT)
    add_library (kiss_fft ${KISS_FFT_LIB_TYPE}
        "src/kiss_fft.c"
        "src/kiss_fftr.c")
    target_compile_options (kiss_fft
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-W -Wall -Wstrict-prototypes -Wmissing-prototypes -Waggregate-return -Wcast-align -Wcast-qual -Wnested-externs -Wshadow -Wbad-function-cast -Wwrite-strings>
            $<$<C_COMPILER_ID:Clang>:-W -Wall -Wstrict-prototypes -Wmissing-prototypes -Waggregate-return -Wcast-align -Wcast-qual -Wnested-externs -Wshadow -Wbad-function-cast -Wwrite-strings>
            $<$<BOOL:${KISS_FFT_USE_SIMD}>:$<$<C_COMPILER_ID:GNU>:-msse>>
            $<$<BOOL:${KISS_FFT_USE_SIMD}>:$<$<C_COMPILER_ID:Clang>:-msse>>)
    target_include_directories (kiss_fft
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            $<INSTALL_INTERFACE:include>)
    target_compile_definitions (kiss_fft
        PRIVATE
            KISS_FFT_BUILDING)
    add_library (KissFFT::kiss_fft ALIAS kiss_fft)
endif ()

###############################################################################
# install targets
###############################################################################

if (${KISS_FFT_LIB_TYPE} MATCHES "SHARED")
    install (
        TARGETS kiss_fft
        LIBRARY DESTINATION "lib" COMPONENT runtime
        RUNTIME DESTINATION "bin" COMPONENT runtime)
endif ()

if (${INSTALL_DEV})
    install (
        TARGETS kiss_fft
        EXPORT KissFFTConfig
        ARCHIVE DESTINATION "lib"
        LIBRARY DESTINATION "lib"
        RUNTIME DESTINATION "bin")
    install (
        DIRECTORY "include/kiss_fft"
        DESTINATION "include"
        FILES_MATCHING PATTERN "*.h")
    install (
        DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/kiss_fft"
        DESTINATION "include"
        FILES_MATCHING PATTERN "*.h")
    install (
        EXPORT KissFFTConfig
        DESTINATION "share/KissFFT/cmake")
endif ()

export (
    TARGETS kiss_fft
    NAMESPACE KissFFT::
    FILE KissFFTConfig.cmake)
export (
    PACKAGE KissFFT)

