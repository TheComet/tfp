cmake_minimum_required (VERSION 3.3)

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition and dump it in the cache along with proper documentation, otherwise
# set MAKE_BUILD_TYPE to Debug prior to calling PROJECT()
if (NOT DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set (SFGSYM_LIB_TYPE "SHARED" CACHE STRING "Build either as SHARED or STATIC library")
set (SFGSYM_REAL "double" CACHE STRING "The type to use for numerical computations")
option (SFGSYM_PIC "Generate position independent code" ON)
option (SFGSYM_TESTS "Build unit tests" ON)

set (NEED_CXX "")
if (SFGSYM_TESTS)
    set (NEED_CXX "CXX")
endif ()

project ("sfgsym"
    VERSION 0.0.1
    LANGUAGES C ${NEED_CXX})

###############################################################################
# CMake includes
###############################################################################

list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

include (GNUInstallDirs)
include (TestVisibilityMacros)

###############################################################################
# Dependencies
###############################################################################

if (TARGET gmock)
    add_library (GTest::gmock ALIAS gmock)
endif ()

###############################################################################
# Config
###############################################################################

## complex hyperbolic functions
#function (check_complex_hyperbolic SYMBOL RESULT_HAVE RESULT_SYMBOL)
#    set (INCLUDES "#include <cmath>\n#include <complex>")
#
#    # First, try to see if it is under std::
#    check_cxx_source_compiles ("
#        ${INCLUDES}\n
#        int main() {
#            std::complex<double> c(1, 4);
#            std::complex<double> res = std::${SYMBOL}(c);
#        }" ${SYMBOL}_RESULT)
#
#    if (${SYMBOL}_RESULT)
#        set (${RESULT_SYMBOL} "std::${SYMBOL}" PARENT_SCOPE)
#        set (${RESULT_HAVE} 1 PARENT_SCOPE)
#    else ()
#        # kay, maybe it's global
#        check_cxx_source_compiles ("
#        ${INCLUDES}
#        int main() {
#            std::complex<double> c(1, 4);
#            std::complex<double> res = ${SYMBOL}(c);
#        }" ${SYMBOL}_RESULT)
#        if (${SYMBOL}_RESULT)
#            set (${RESULT_SYMBOL} "${SYMBOL}" PARENT_SCOPE)
#            set (${RESULT_HAVE} 1 PARENT_SCOPE)
#        else ()
#            set (${RESULT_SYMBOL} "sfgsym::${SYMBOL}" PARENT_SCOPE)
#            set (${RESULT_HAVE} 0 PARENT_SCOPE)
#        endif ()
#    endif ()
#endfunction ()
#
#check_complex_hyperbolic ("sinh" SFGSYM_HAVE_SINH SINH_SYMBOL)
#check_complex_hyperbolic ("cosh" SFGSYM_HAVE_COSH COSH_SYMBOL)
#check_complex_hyperbolic ("tanh" SFGSYM_HAVE_TANH TANH_SYMBOL)
#check_complex_hyperbolic ("asinh" SFGSYM_HAVE_ASINH ASINH_SYMBOL)
#check_complex_hyperbolic ("acosh" SFGSYM_HAVE_ACOSH ACOSH_SYMBOL)
#check_complex_hyperbolic ("atanh" SFGSYM_HAVE_ATANH ATANH_SYMBOL)

test_visibility_macros (
    SFGSYM_API_EXPORT
    SFGSYM_API_IMPORT
    SFGSYM_API_LOCAL)

configure_file (
    "templates/config.h.in"
    "include/sfgsym/config.h")

###############################################################################
# Math expression parser
###############################################################################

find_package (FLEX 2.6 REQUIRED)
find_package (BISON 3.7 REQUIRED)

# These may not exist
file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src/parsers")
file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/sfgsym/parsers")

bison_target (sfgsym_expression_parser
    "${PROJECT_SOURCE_DIR}/src/parsers/expression_parser.y"
    "${PROJECT_BINARY_DIR}/src/parsers/expression_parser.y.c"
    DEFINES_FILE "${PROJECT_BINARY_DIR}/include/sfgsym/parsers/expression_parser.y.h"
    COMPILE_FLAGS -Wcounterexamples)
flex_target (sfgsym_expression_lexer
    "${PROJECT_SOURCE_DIR}/src/parsers/expression_parser.l"
    "${PROJECT_BINARY_DIR}/src/parsers/expression_parser.l.c")
add_flex_bison_dependency (sfgsym_expression_lexer sfgsym_expression_parser)

###############################################################################
# Library definition
###############################################################################

set (SFGSYM_SOURCES
    "src/algo/find_paths.c"
    "src/algo/mason.c"
    "src/exporters/graph2dot.c"
    "src/exporters/expr2dot.c"
    "src/sfg/branch.c"
    "src/sfg/graph.c"
    "src/sfg/node.c"
    "src/sfg/path.c"
    "src/symbolic/expression.c"
    "src/symbolic/expression_eval.c"
    "src/symbolic/expression_constant_fold.c"
    "src/symbolic/expression_normalize.c"
    "src/symbolic/expression_factor.c"
    "src/symbolic/subs_table.c"
    "src/util/log.c"
    "src/init.c")
set (SFGSYM_PARSER_SOURCES
    ${BISON_sfgsym_expression_parser_OUTPUTS}
    ${FLEX_sfgsym_expression_lexer_OUTPUTS}
    "src/parsers/expression_parser.y"
    "src/parsers/expression_parser.l"
    "src/parsers/expression_parser.c"
    "src/parsers/expression_parser_replace_exp_function.c"
    "src/parsers/expression_parser_collapse_lists.c")
set (SFGSYM_TEST_SOURCES
    "tests/src/test_expression.cpp"
    "tests/src/test_expression_parser.cpp"
    "tests/src/test_expression_factor_add_sub.cpp"
    "tests/src/test_expression_simplify_redundant_ops.cpp"
    "tests/src/test_algo_find_loops.cpp"
    "tests/src/test_algo_find_paths.cpp"
    "tests/src/test_algo_mason.cpp"
    "tests/src/run_tests.cpp")

add_library (sfgsym ${SFGSYM_LIB_TYPE}
    ${SFGSYM_SOURCES}
    ${SFGSYM_PARSER_SOURCES}
    $<$<BOOL:${SFGSYM_TESTS}>:${SFGSYM_TEST_SOURCES}>)
set_target_properties (sfgsym
    PROPERTIES
        C_STANDARD 90
        POSITION_INDEPENDENT_CODE ${SFGSYM_PIC})
target_include_directories (sfgsym
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<$<BOOL:${SFGSYM_TESTS}>:$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/tests/include>>
        $<INSTALL_INTERFACE:include>)
target_compile_options (sfgsym
    PRIVATE
    $<$<C_COMPILER_ID:GNU>:-W -Wall -Wextra -pedantic -Wshadow>
    $<$<C_COMPILER_ID:Clang>:-W -Wall -Wextra -pedantic -Wshadow>)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set_source_files_properties (${FLEX_sfgsym_expression_lexer_OUTPUTS}
        PROPERTIES COMPILE_OPTIONS "-Wno-unused-function;-Wno-pedantic")
    set_source_files_properties (${BISON_sfgsym_expression_parser_OUTPUTS}
        PROPERTIES COMPILE_OPTIONS "-Wno-pedantic")
endif ()
target_compile_definitions (sfgsym
    PRIVATE
        SFGSYM_BUILDING)
target_link_libraries (sfgsym
    PRIVATE
        cstructures
        $<$<BOOL:${SFGSYM_TESTS}>:GTest::gmock>)
set_target_properties (sfgsym
    PROPERTIES
        SOVERSION ${PROJECT_VERSION})

if (SFGSYM_TESTS)
    if (SFGSYM_PIC)
        set_target_properties (gmock
            PROPERTIES POSITION_INDEPENDENT_CODE ON)
        set_target_properties (gtest
            PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif ()

    add_executable (sfgsym_tests
        "tests/src/main.c")
    target_link_libraries (sfgsym_tests
        PRIVATE sfgsym)
    set_target_properties (sfgsym_tests
        PROPERTIES INSTALL_RPATH "../lib")
endif ()

###############################################################################
# Install target
###############################################################################

#if (${SFGSYM_LIB_TYPE} MATCHES "SHARED")
#    install (
#        TARGETS sfgsym
#        LIBRARY DESTINATION "lib" COMPONENT runtime
#        RUNTIME DESTINATION "bin" COMPONENT runtime)
#endif ()
#
#if (${SFGSYM_TESTS})
#    install (
#        TARGETS sfgsym_tests
#        RUNTIME DESTINATION "bin")
#endif ()
#
#if (${INSTALL_DEV})
#    install (
#        TARGETS sfgsym
#        EXPORT SFGSYMConfig
#        ARCHIVE DESTINATION "lib"
#        LIBRARY DESTINATION "lib"
#        RUNTIME DESTINATION "bin")
#    install (
#        DIRECTORY "include/sfgsym"
#        DESTINATION "include"
#        FILES_MATCHING PATTERN "*.h")
#    install (
#        DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/sfgsym"
#        DESTINATION "include"
#        FILES_MATCHING PATTERN "*.h")
#    install (
#        EXPORT SFGSYMConfig
#        DESTINATION "share/sfgsym/cmake")
#endif ()
#
#set (SFGSYM_targets sfgsym)
#if (${SFGSYM_TESTS})
#    list (APPEND SFGSYM_targets sfgsym_tests)
#endif ()
#
#export (
#    TARGETS ${SFGSYM_targets}
#    NAMESPACE SFGSYM::
#    FILE SFGSYMConfig.cmake)
#export (
#    PACKAGE SFGSYM)

