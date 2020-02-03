cmake_minimum_required (VERSION 3.3)

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition and dump it in the cache along with proper documentation, otherwise
# set MAKE_BUILD_TYPE to Debug prior to calling PROJECT()
if (NOT DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

project ("sfgsym"
    VERSION 0.0.1
    LANGUAGES CXX)

###############################################################################
# CMake includes
###############################################################################

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

include (GNUInstallDirs)
include (TestVisibilityMacros)
include (CheckCXXSourceCompiles)

###############################################################################
# Dependencies
###############################################################################

find_package (FLEX REQUIRED)
find_package (BISON REQUIRED)

if (TARGET gmock)
    add_library (GTest::gmock ALIAS gmock)
endif ()

###############################################################################
# Config
###############################################################################

set (SFGSYM_LIB_TYPE "SHARED" CACHE STRING "Build either as SHARED or STATIC library")
set (SFGSYM_REAL "double" CACHE STRING "The type to use for numerical computations")
option (SFGSYM_TESTS "Build unit tests" OFF)

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
            set (${RESULT_SYMBOL} "sfgsym::${SYMBOL}" PARENT_SCOPE)
            set (${RESULT_HAVE} 0 PARENT_SCOPE)
        endif ()
    endif ()
endfunction ()

check_complex_hyperbolic ("sinh" SFGSYM_HAVE_SINH SINH_SYMBOL)
check_complex_hyperbolic ("cosh" SFGSYM_HAVE_COSH COSH_SYMBOL)
check_complex_hyperbolic ("tanh" SFGSYM_HAVE_TANH TANH_SYMBOL)
check_complex_hyperbolic ("asinh" SFGSYM_HAVE_ASINH ASINH_SYMBOL)
check_complex_hyperbolic ("acosh" SFGSYM_HAVE_ACOSH ACOSH_SYMBOL)
check_complex_hyperbolic ("atanh" SFGSYM_HAVE_ATANH ATANH_SYMBOL)

test_visibility_macros (SFGSYM_API_EXPORT SFGSYM_API_IMPORT SFGSYM_API_LOCAL)

configure_file ("templates/config.hpp.in" "include/sfgsym/config.hpp")

###############################################################################
# Math expression parser
###############################################################################

file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src/symbolic")
file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/sfgsym/symbolic")
bison_target (SFGSYMExpressionParser
    "${CMAKE_CURRENT_LIST_DIR}/src/symbolic/ExpressionParser.y"
    "${CMAKE_CURRENT_BINARY_DIR}/src/symbolic/ExpressionParser.cpp"
    DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/sfgsym/symbolic/ExpressionParser.hpp")
flex_target (SFGSYMExpressionLexer
    "${CMAKE_CURRENT_LIST_DIR}/src/symbolic/ExpressionLexer.l"
    "${CMAKE_CURRENT_BINARY_DIR}/src/symbolic/ExpressionLexer.cpp")
add_flex_bison_dependency (SFGSYMExpressionLexer SFGSYMExpressionParser)

###############################################################################
# Library definition
###############################################################################

add_library (sfgsym ${SFGSYM_LIB_TYPE}
    ${BISON_SFGSYMExpressionParser_OUTPUTS}
    ${FLEX_SFGSYMExpressionLexer_OUTPUTS}
    "src/sfg/Connection.cpp"
    "src/sfg/Graph.cpp"
    "src/sfg/Node.cpp"
    "src/symbolic/Expression.cpp"
    "src/util/Math.cpp"
    "src/util/Metric.cpp"
    "src/util/RefCounted.cpp"
    "src/util/String.cpp")
target_include_directories (sfgsym
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_compile_options (sfgsym
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-W -Wall -Wextra -pedantic>
        $<$<CXX_COMPILER_ID:Clang>:-W -Wall -Wextra -pedantic>)
target_compile_definitions (sfgsym
    PRIVATE
        SFGSYM_BUILDING)
target_link_libraries (sfgsym
    PRIVATE
        KissFFT::kiss_fft)

if (${SFGSYM_TESTS})
    add_library (sfgsym_tests_obj OBJECT
        "tests/test_Expression.cpp")
    target_link_libraries (sfgsym_tests_obj
        PUBLIC
            sfgsym
            GTest::gmock)
    add_executable (sfgsym_tests
        "tests/main.cpp")
    target_link_libraries (sfgsym_tests
        PUBLIC
            $<TARGET_OBJECTS:sfgsym_tests_obj>
            sfgsym
            GTest::gmock)
    set_target_properties (sfgsym_tests PROPERTIES INSTALL_RPATH "../lib")
endif ()

###############################################################################
# Install target
###############################################################################

if (${SFGSYM_LIB_TYPE} MATCHES "SHARED")
    install (
        TARGETS sfgsym
        LIBRARY DESTINATION "lib" COMPONENT runtime
        RUNTIME DESTINATION "bin" COMPONENT runtime)
endif ()

if (${SFGSYM_TESTS})
    install (
        TARGETS sfgsym_tests
        RUNTIME DESTINATION "bin")
endif ()

if (${INSTALL_DEV})
    install (
        TARGETS sfgsym
        EXPORT SFGSYMConfig
        ARCHIVE DESTINATION "lib"
        LIBRARY DESTINATION "lib"
        RUNTIME DESTINATION "bin")
    install (
        DIRECTORY "include/sfgsym"
        DESTINATION "include"
        FILES_MATCHING PATTERN "*.h")
    install (
        DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/sfgsym"
        DESTINATION "include"
        FILES_MATCHING PATTERN "*.h")
    install (
        EXPORT SFGSYMConfig
        DESTINATION "share/sfgsym/cmake")
endif ()

set (SFGSYM_targets sfgsym)
if (${SFGSYM_TESTS})
    list (APPEND SFGSYM_targets sfgsym_tests)
endif ()

export (
    TARGETS ${SFGSYM_targets}
    NAMESPACE SFGSYM::
    FILE SFGSYMConfig.cmake)
export (
    PACKAGE SFGSYM)
