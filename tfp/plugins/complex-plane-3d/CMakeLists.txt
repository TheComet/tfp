###############################################################################
# source files and library definition
###############################################################################

set (cp3d_HEADERS_MOC "")

set (cp3d_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ComplexPlane3D.cpp")
set (cp3d_FORMS "")
set (cp3d_RESOURCES "")

set (cp3d_TRANSLATION_FILES "")

qt5_wrap_cpp (cp3d_SOURCES_MOC ${cp3d_HEADERS_MOC})
qt5_wrap_ui  (cp3d_FORMS_MOC ${cp3d_FORMS})
qt5_add_resources (cp3d_RESOURCES_GEN ${cp3d_RESOURCES})
#qt5_add_translation (cp3d_QM_FILES ${cp3d_TRANSLATION_FILES})

#include_directories ("${CMAKE_CURRENT_LIST_DIR}")
include_directories (${CMAKE_BINARY_DIR}) # need this because MOC outputs to binary_dir/pzp

add_definitions (-DCOMPLEX_PLANE_3D_BUILDING)

add_library (complex-plane-3d SHARED
    ${cp3d_SOURCES}
    ${cp3d_SOURCES_MOC}
    ${cp3d_FORMS_MOC}
    ${cp3d_RESOURCES_GEN}
    ${QM_FILES}
)

set_target_properties (complex-plane-3d PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
    PREFIX "")

add_dependencies (tfp complex-plane-3d)

###############################################################################
# dependencies
###############################################################################

target_link_libraries (complex-plane-3d Qt5::Core)
target_link_libraries (complex-plane-3d Qt5::Widgets)
target_link_libraries (complex-plane-3d Qt5::Gui)
target_link_libraries (complex-plane-3d Qt5::Concurrent)
target_link_libraries (complex-plane-3d Qt5::PrintSupport)
target_link_libraries (complex-plane-3d Qt5::OpenGL)
target_link_libraries (complex-plane-3d Qt5::Svg)

target_link_libraries (complex-plane-3d qwtinternal)
target_link_libraries (complex-plane-3d qwtplot3d)
target_link_libraries (complex-plane-3d qwtmathml)
target_link_libraries (complex-plane-3d kiss_fft)
target_link_libraries (complex-plane-3d pugixml)
