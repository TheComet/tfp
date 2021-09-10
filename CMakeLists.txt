cmake_minimum_required (VERSION 3.8)

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

project ("TFP"
    VERSION 0.0.1
    LANGUAGES C CXX)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

###############################################################################
# Options
###############################################################################

# must be in this order due to linking
add_subdirectory ("thirdparty/googletest-1.10.0")
#add_subdirectory ("thirdparty/kiss_fft130")
add_subdirectory ("sfgsym")
#add_subdirectory ("thirdparty/qwt-6.1.2")
#add_subdirectory ("thirdparty/qwtplot3d-0.3.1")
#add_subdirectory ("thirdparty/ads-2.0.1")
#add_subdirectory ("tfp")
#add_subdirectory ("application")
#if (TFP_TESTS)
#    add_subdirectory ("thirdparty/googletest")
#endif ()
