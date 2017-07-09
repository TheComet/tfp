###############################################################################
# source files and library definition
###############################################################################

set (bode_HEADERS_MOC "")

set (bode_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/BodePlot.cpp")
set (bode_FORMS "")
set (bode_RESOURCES "")

set (bode_TRANSLATION_FILES "")

qt5_wrap_cpp (bode_SOURCES_MOC ${bode_HEADERS_MOC})
qt5_wrap_ui  (bode_FORMS_MOC ${bode_FORMS})
qt5_add_resources (bode_RESOURCES_GEN ${bode_RESOURCES})
#qt5_add_translation (bode_QM_FILES ${bode_TRANSLATION_FILES})

#include_directories ("${CMAKE_CURRENT_LIST_DIR}")
include_directories (${CMAKE_BINARY_DIR}) # need this because MOC outputs to binary_dir/pzp

add_definitions (-DBODE_PLOT_BUILDING)

add_library (bode-plot SHARED
    ${bode_SOURCES}
    ${bode_SOURCES_MOC}
    ${bode_FORMS_MOC}
    ${bode_RESOURCES_GEN}
    ${QM_FILES}
)

set_target_properties (bode-plot PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
    PREFIX "")

add_dependencies (tfp bode-plot)

###############################################################################
# dependencies
###############################################################################

target_link_libraries (bode-plot Qt5::Core)
target_link_libraries (bode-plot Qt5::Widgets)
target_link_libraries (bode-plot Qt5::Gui)
target_link_libraries (bode-plot Qt5::Concurrent)
target_link_libraries (bode-plot Qt5::PrintSupport)
target_link_libraries (bode-plot Qt5::OpenGL)
target_link_libraries (bode-plot Qt5::Svg)

target_link_libraries (bode-plot qwtinternal)
target_link_libraries (bode-plot qwtplot3d)
target_link_libraries (bode-plot qwtmathml)
target_link_libraries (bode-plot kiss_fft)
target_link_libraries (bode-plot pugixml)
