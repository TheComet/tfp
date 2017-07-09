###############################################################################
# source files and library definition
###############################################################################

set (pzp_HEADERS_MOC "")

set (pzp_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/PoleZeroPlot.cpp")
set (pzp_FORMS "")
set (pzp_RESOURCES "")

set (pzp_TRANSLATION_FILES "")

qt5_wrap_cpp (pzp_SOURCES_MOC ${pzp_HEADERS_MOC})
qt5_wrap_ui  (pzp_FORMS_MOC ${pzp_FORMS})
qt5_add_resources (pzp_RESOURCES_GEN ${pzp_RESOURCES})
#qt5_add_translation (pzp_QM_FILES ${pzp_TRANSLATION_FILES})

#include_directories ("${CMAKE_CURRENT_LIST_DIR}")
include_directories (${CMAKE_BINARY_DIR}) # need this because MOC outputs to binary_dir/pzp

add_definitions (-DPOLE_ZERO_PLOT_BUILDING)

add_library (pole-zero-plot SHARED
    ${pzp_SOURCES}
    ${pzp_SOURCES_MOC}
    ${pzp_FORMS_MOC}
    ${pzp_RESOURCES_GEN}
    ${QM_FILES}
)

set_target_properties (pole-zero-plot PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
    PREFIX "")

add_dependencies (tfp pole-zero-plot)

###############################################################################
# dependencies
###############################################################################

target_link_libraries (pole-zero-plot Qt5::Core)
target_link_libraries (pole-zero-plot Qt5::Widgets)
target_link_libraries (pole-zero-plot Qt5::Gui)
target_link_libraries (pole-zero-plot Qt5::Concurrent)
target_link_libraries (pole-zero-plot Qt5::PrintSupport)
target_link_libraries (pole-zero-plot Qt5::OpenGL)
target_link_libraries (pole-zero-plot Qt5::Svg)

target_link_libraries (pole-zero-plot qwtinternal)
target_link_libraries (pole-zero-plot qwtplot3d)
target_link_libraries (pole-zero-plot qwtmathml)
target_link_libraries (pole-zero-plot kiss_fft)
target_link_libraries (pole-zero-plot pugixml)
