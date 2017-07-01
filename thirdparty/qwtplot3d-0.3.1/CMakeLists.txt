###############################################################################
# compiler flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

###############################################################################
# config
###############################################################################

set (QWTPLOT3D_VER_MAJ 0)
set (QWTPLOT3D_VER_MIN 3)
set (QWTPLOT3D_VER_PAT 1)
set (QWTPLOT3D_VERSION "${QWTPLOT3D_VER_MAJ}.${QWTPLOT3D_VER_MIN}.${QWTPLOT3D_VER_PAT}")

option (QWTPLOT3D_SHARED_LIB "Enable this to build Qwt as a shared library. Disable to build as a static library" ON)

message (STATUS "------------------------------------------------------------")
message (STATUS "qwtplot3d settings")
message (STATUS " + Build as a shared library: ${QWTPLOT3D_SHARED_LIB}")
message (STATUS "------------------------------------------------------------")

###############################################################################
# sub directories
###############################################################################

add_subdirectory ("qwtplot3d")

