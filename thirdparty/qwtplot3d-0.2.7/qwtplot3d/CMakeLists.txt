###############################################################################
# compiler flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-Wno-error -fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-Wno-error -fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

###############################################################################
# Defines
###############################################################################

if (QWTPLOT3D_SHARED)
    add_definitions (-DQT_DLL)
    add_definitions (-DQWT3D_DLL)
    add_definitions (-DQWT3D_MAKEDLL)
endif ()

###############################################################################
# Library, source and header files
###############################################################################

set (QWTPLOT3D_SOURCES
    "src/qwt3d_axis.cpp"
    "src/qwt3d_color.cpp"
    "src/qwt3d_coordsys.cpp"
    "src/qwt3d_drawable.cpp"
    "src/qwt3d_mousekeyboard.cpp"
    "src/qwt3d_movements.cpp"
    "src/qwt3d_lighting.cpp"
    "src/qwt3d_colorlegend.cpp"
    "src/qwt3d_plot.cpp"
    "src/qwt3d_label.cpp"
    "src/qwt3d_types.cpp"
    "src/qwt3d_enrichment_std.cpp"
    "src/qwt3d_autoscaler.cpp"
    "src/qwt3d_io_reader.cpp"
    "src/qwt3d_io.cpp"
    "src/qwt3d_scale.cpp")
set (QWTPLOT3D_SOURCES ${QWTPLOT3D_SOURCES}
    "src/qwt3d_gridmapping.cpp"
    "src/qwt3d_parametricsurface.cpp"
    "src/qwt3d_function.cpp")
set (QWTPLOT3D_SOURCES ${QWTPLOT3D_SOURCES}
    "src/qwt3d_surfaceplot.cpp"
    "src/qwt3d_gridplot.cpp"
    "src/qwt3d_meshplot.cpp")

set (QWTPLOT3D_MOC_HEADERS
    "include/qwt3d_plot.h"
    "include/qwt3d_surfaceplot.h")

# gl2ps support
set (QWTPLOT3D_SOURCES ${QWTPLOT3D_SOURCES}
    "src/qwt3d_io_gl2ps.cpp"
    "../3rdparty/gl2ps/gl2ps.c")

qt5_wrap_cpp (QWTPLOT3D_MOC_SOURCES ${QWTPLOT3D_MOC_HEADERS})

set (LIBRARY_NAME QWTPLOT3D)
if (QWTPLOT3D_SHARED_LIB)
    set (BUILD_TYPE SHARED)
else ()
    set (BUILD_TYPE STATIC)
endif ()

list (APPEND QWTPLOT3D_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
unset (QWTPLOT3D_INCLUDE_DIRS CACHE)
set (QWTPLOT3D_INCLUDE_DIRS ${QWTPLOT3D_INCLUDE_DIRS} CACHE STRING "Qwt include directories")
include_directories (${QWTPLOT3D_INCLUDE_DIRS})

add_library (qwtplot3d ${BUILD_TYPE}
    ${QWTPLOT3D_SOURCES}
    ${QWTPLOT3D_MOC_SOURCES}
)
unset (QWTPLOT3D_LIBRARIES CACHE)
set (QWTPLOT3D_LIBRARIES qwtplot3d CACHE STRING "qwtplot3d libraries")

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
create_vcproj_userfile (qwtplot3d)

target_link_libraries (qwtplot3d Qt5::Core Qt5::Gui Qt5::OpenGL)

find_package (OpenGL REQUIRED)
include_directories (${OPENGL_INCLUDE_DIR})
target_link_libraries (qwtplot3d ${OPENGL_glu_LIBRARY})

find_package (ZLIB REQUIRED)
include_directories (${ZLIB_INCLUDE_DIRS})
target_link_libraries (qwtplot3d ${ZLIB_LIBRARIES})

###############################################################################
# install targets
###############################################################################

install (
    TARGETS qwtplot3d
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)

