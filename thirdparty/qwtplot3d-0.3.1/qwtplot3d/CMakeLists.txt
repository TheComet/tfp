###############################################################################
# config
###############################################################################

configure_file ("include/qwt3d_global.h.in" "include/qwt3d_global.h")

###############################################################################
# Library, source and header files
###############################################################################

set (QWTPLOT3D_SOURCES
    "src/qwt3d_appearance.cpp"
    "src/qwt3d_autoscaler.cpp"
    "src/qwt3d_axis.cpp"
    "src/qwt3d_color_std.cpp"
    "src/qwt3d_colorlegend.cpp"
    "src/qwt3d_coordsys.cpp"
    "src/qwt3d_data.cpp"
    "src/qwt3d_drawable.cpp"
    "src/qwt3d_enrichment_std.cpp"
    "src/qwt3d_extglwidget.cpp"
    "src/qwt3d_function.cpp"
    "src/qwt3d_graphplot.cpp"
    "src/qwt3d_gridmapping.cpp"
    "src/qwt3d_gridplot.cpp"
    "src/qwt3d_io.cpp"
    "src/qwt3d_io_reader.cpp"
    "src/qwt3d_label.cpp"
    "src/qwt3d_lighting.cpp"
    "src/qwt3d_meshplot.cpp"
    "src/qwt3d_parametricsurface.cpp"
    "src/qwt3d_plot3d.cpp"
    "src/qwt3d_scale.cpp"
    "src/qwt3d_surfaceplot.cpp"
    "src/qwt3d_types.cpp"
    "src/qwt3d_volumeplot.cpp")
set (QWTPLOT3D_MOC_HEADERS
    "include/qwt3d_plot3d.h"
    "include/qwt3d_extglwidget.h"
    "include/qwt3d_gridplot.h"
    "include/qwt3d_surfaceplot.h")

find_package (Qt5 COMPONENTS Core Gui OpenGL REQUIRED)
find_package (OpenGL REQUIRED)

qt5_wrap_cpp (QWTPLOT3D_MOC_SOURCES ${QWTPLOT3D_MOC_HEADERS})

if (QWTPLOT3D_SHARED_LIB)
    set (BUILD_TYPE SHARED)
else ()
    set (BUILD_TYPE STATIC)
endif ()

add_library (qwtplot3d ${BUILD_TYPE}
    ${QWTPLOT3D_SOURCES}
    ${QWTPLOT3D_MOC_SOURCES})
target_compile_options (qwtplot3d
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-W -Wall -Wextra -pedantic -Wno-unused-parameter>
        $<$<CXX_COMPILER_ID:Clang>:-W -Wall -Wextra -pedantic -Wno-unused-parameter>)
target_compile_definitions (qwtplot3d
    PRIVATE
        QWTPLOT3D_BUILDING)
target_link_libraries (qwtplot3d
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::OpenGL
        OpenGL::GL)
target_include_directories (qwtplot3d
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

###############################################################################
# install targets
###############################################################################

install (
    TARGETS qwtplot3d
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)
