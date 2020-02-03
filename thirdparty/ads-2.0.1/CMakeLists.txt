cmake_minimum_required (VERSION 3.3)

project ("ads"
    VERSION 2.0.1
    LANGUAGES CXX)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

###############################################################################
# config
###############################################################################

set (ADS_LIB_TYPE "STATIC" CACHE STRING "Build ADS as a SHARED library or as a STATIC library")
option (ADS_DEMO "whether to build the demo or not" ON)

message (STATUS "------------------------------------------------------------")
message (STATUS "ADS settings")
message (STATUS " + Lib type: ${ADS_LIB_TYPE}")
message (STATUS " + Build demo: ${ADS_DEMO}")
message (STATUS "------------------------------------------------------------")

###############################################################################
# sub directories
###############################################################################

add_subdirectory ("ads")
if (ADS_DEMO)
    add_subdirectory ("demo")
endif ()
