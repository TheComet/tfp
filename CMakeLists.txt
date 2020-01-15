cmake_minimum_required (VERSION 3.8.0)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition and dump it in the cache along with proper documentation, otherwise
# set MAKE_BUILD_TYPE to Debug prior to calling PROJECT()
if (NOT DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
message (STATUS "Build type: ${CMAKE_BUILD_TYPE}")

###############################################################################
# Project name
###############################################################################

project ("Transfer-Function-Plotter"
    VERSION 0.0.1
    LANGUAGES CXX)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")

#include (CMakeToolsHelpers OPTIONAL)
include (GNUInstallDirs)
include (TestVisibilityMacros)
include (ConfigureMSVCRuntime)


###############################################################################
# set output locations
###############################################################################

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

foreach (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
endforeach (OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

###############################################################################
# Options
###############################################################################

set (TFP_REAL "double" CACHE STRING "The type to use for numerical computations")
set (TFP_LIB_TYPE "SHARED" CACHE STRING "Either SHARED or STATIC")
option (TFP_TESTS "Whether to build unit tests" ON)

# complex hyperbolic functions
function (check_complex_hyperbolic SYMBOL RESULT_HAVE RESULT_SYMBOL)
    set (INCLUDES "#include <cmath>\n#include <complex>")

    # First, try to see if it is under std::
    check_cxx_source_compiles ("
        ${INCLUDES}\n
        int main() {
            std::complex<double> c(1, 4);
            std::complex<double> res = std::${SYMBOL}(c);
        }" ${SYMBOL}_RESULT)

    if (${SYMBOL}_RESULT)
        set (${RESULT_SYMBOL} "std::${SYMBOL}" PARENT_SCOPE)
        set (${RESULT_HAVE} 1 PARENT_SCOPE)
    else ()
        # kay, maybe it's global
        check_cxx_source_compiles ("
        ${INCLUDES}
        int main() {
            std::complex<double> c(1, 4);
            std::complex<double> res = ${SYMBOL}(c);
        }" ${SYMBOL}_RESULT)
        if (${SYMBOL}_RESULT)
            set (${RESULT_SYMBOL} "${SYMBOL}" PARENT_SCOPE)
            set (${RESULT_HAVE} 1 PARENT_SCOPE)
        else ()
            set (${RESULT_SYMBOL} "tfp::${SYMBOL}" PARENT_SCOPE)
            set (${RESULT_HAVE} 0 PARENT_SCOPE)
        endif ()
    endif ()
endfunction ()

check_complex_hyperbolic ("sinh" TFP_HAVE_SINH SINH_SYMBOL)
check_complex_hyperbolic ("cosh" TFP_HAVE_COSH COSH_SYMBOL)
check_complex_hyperbolic ("tanh" TFP_HAVE_TANH TANH_SYMBOL)
check_complex_hyperbolic ("asinh" TFP_HAVE_ASINH ASINH_SYMBOL)
check_complex_hyperbolic ("acosh" TFP_HAVE_ACOSH ACOSH_SYMBOL)
check_complex_hyperbolic ("atanh" TFP_HAVE_ATANH ATANH_SYMBOL)

test_visibility_macros (TFP_API_EXPORT TFP_API_IMPORT TFP_API_LOCAL)

message (STATUS "------------------------------------------------------------")
message (STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")
message (STATUS "TFP lib type: ${TFP_LIB_TYPE}")
message (STATUS "Real type: ${TFP_REAL}")
message (STATUS "Build tests: ${TFP_TESTS}")
message (STATUS "------------------------------------------------------------")

# must be in this order due to linking
add_subdirectory ("thirdparty/kiss_fft130")
add_subdirectory ("thirdparty/pugixml-1.7")
add_subdirectory ("thirdparty/qwt-6.1.2")
add_subdirectory ("thirdparty/qwtplot3d-0.3.1")
add_subdirectory ("thirdparty/ads-2.0.1")
add_subdirectory ("tfp")
add_subdirectory ("application")
if (TFP_TESTS)
    add_subdirectory ("thirdparty/googlemock")
endif ()
