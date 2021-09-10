include (GNUInstallDirs)

###############################################################################
# Options and config
###############################################################################

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set (DEBUG_FEATURE ON)
endif ()

set (CSTRUCTURES_LIB_TYPE "STATIC" CACHE STRING "Build as shared or static")
set (CSTRUCTURES_BTREE_EXPAND_FACTOR "2" CACHE STRING "When reallocating btree memory, this is the factor with which the buffer grows")
set (CSTRUCTURES_BTREE_MIN_CAPACITY "32" CACHE STRING "The smallest number of elements to reserve when initializing a btree")
set (CSTRUCTURES_VEC_EXPAND_FACTOR "2" CACHE STRING "When reallocating vector memory, this is the factor with which the buffer grows")
set (CSTRUCTURES_VEC_MIN_CAPACITY "32" CACHE STRING "The smallest number of elements to reserve when initializing a vector")
option (CSTRUCTURES_BTREE_64BIT_KEYS "Enable 64-bit keys for btrees instead of 32-bit keys" OFF)
option (CSTRUCTURES_BTREE_64BIT_CAPACITY "Enable btrees to allow up to 2^64 entries instead of 2^32" OFF)
option (CSTRUCTURES_BENCHMARKS "Compile benchmarks (requires C++)" OFF)
option (CSTRUCTURES_MEMORY_BACKTRACE "Enable generating backtraces to every malloc/realloc call, making it easy to find where memory leaks occur" ${DEBUG_FEATURE})
option (CSTRUCTURES_MEMORY_DEBUGGING "Enable tracking malloc/realloc/free calls to detect memory leaks" ${DEBUG_FEATURE})
option (CSTRUCTURES_PIC "Generate position independent code" ON)
option (CSTRUCTURES_PROFILING "Enable -pg and -fno-omit-frame-pointer" OFF)
option (CSTRUCTURES_TESTS "Compile unit tests (requires C++)" OFF)
option (CSTRUCTURES_VEC_64BIT "Set vector capacity to 2^64 instead of 2^32, but makes the structure 32 bytes instead of 20 bytes" OFF)

if (${CSTRUCTURES_TESTS} OR ${CSTRUCTURES_BENCHMARKS})
    set (NEED_CXX CXX)
endif ()

project ("C Structures"
    VERSION 0.0.1
    LANGUAGES C ${NEED_CXX})

configure_file ("templates/config.h.in" "include/cstructures/config.h")

###############################################################################
# Library source files and settings
###############################################################################

add_library (cstructures ${CSTRUCTURES_LIB_TYPE}
    "src/btree.c"
    "src/hash.c"
    "src/hashmap.c"
    "src/init.c"
    "src/memory.c"
    "src/string.c"
    "src/vector.c"
    $<$<PLATFORM_ID:Linux>:src/platform/linux/backtrace_linux.c>)
set_property (TARGET cstructures
    PROPERTY POSITION_INDEPENDENT_CODE ${CSTRUCTURES_PIC})
target_include_directories (cstructures
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_compile_options (cstructures
    PRIVATE $<$<C_COMPILER_ID:MSVC>:
        /EHa /MTd /W4 /wd4305 /wd4201 /wd4706 /wd4100 /wd4244 /wd4477 /wd4003 /D_CRT_SECURE_NO_DEPRECATE
    >
    PRIVATE $<$<C_COMPILER_ID:GNU>:
        -W -Wall -Wextra -Wshadow -Wconversion -Wno-unused-parameter
        $<$<BOOL:${CSTRUCTURES_PROFILING}>:-pg -fno-omit-frame-pointer>
    >
    PRIVATE $<$<C_COMPILER_ID:Clang>:
        -W -Wall -Wextra -Wshadow -Wconversion -Wno-unused-parameter
        $<$<BOOL:${CSTRUCTURES_PROFILING}>:-pg -fno-omit-frame-pointer>
    >)

###############################################################################
# Unit tests
###############################################################################

if (CSTRUCTURES_TESTS)
    add_executable (cstructures_tests
        "src/tests/test_btree.cpp"
        "src/tests/test_btree_as_set.cpp"
        "src/tests/test_hashmap.cpp"
        "src/tests/test_vector.cpp"
        "src/tests/env_library_init.cpp"
        "src/tests/main.cpp")
    target_link_libraries (cstructures_tests PUBLIC cstructures)
    target_link_libraries (cstructures_tests PUBLIC gmock)
endif ()

###############################################################################
# Benchmarks
###############################################################################

if (CSTRUCTURES_BENCHMARKS)
    add_executable (cstructures_benchmarks
        "src/benchmarks/bench_hashmap.cpp"
        "src/benchmarks/bench_std_unordered_map.cpp"
        "src/benchmarks/bench_vector.cpp"
        "src/benchmarks/bench_std_vector.cpp"
        "src/benchmarks/main.cpp")
    target_link_libraries (cstructures_benchmarks PUBLIC cstructures)
    target_link_libraries (cstructures_benchmarks PUBLIC benchmark)
    target_include_directories (cstructures_benchmarks
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/benchmark/include>)
endif ()

###############################################################################
# Installation
###############################################################################

# make install to correct locations provided by GNUInstallDirs
install (
    TARGETS
        cstructures
    EXPORT
        CStructuresConfig
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install (
    DIRECTORY
        "include/cstructures"
        "${CMAKE_CURRENT_BINARY_DIR}/include/cstructures"
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR})

# Makes the project importable from the build directory
export (
    TARGETS
        cstructures
    FILE
        CStructuresConfig.cmake)

# Makes the project importable from the install directory
install (
    EXPORT CStructuresConfig
    DESTINATION "share/cstructures/cmake")
