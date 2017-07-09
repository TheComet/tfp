###############################################################################
# source files and library definition
###############################################################################

set (stdfilt_HEADERS_MOC
        "${CMAKE_CURRENT_LIST_DIR}/StandardLowOrderFilter.hpp")

set (stdfilt_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/StandardLowOrderFilter.cpp")
set (stdfilt_FORMS "")
set (stdfilt_RESOURCES "")

set (stdfilt_TRANSLATION_FILES "")

qt5_wrap_cpp (stdfilt_SOURCES_MOC ${stdfilt_HEADERS_MOC})
qt5_wrap_ui  (stdfilt_FORMS_MOC ${stdfilt_FORMS})
qt5_add_resources (stdfilt_RESOURCES_GEN ${stdfilt_RESOURCES})
#qt5_add_translation (stdfilt_QM_FILES ${stdfilt_TRANSLATION_FILES})

#include_directories ("${CMAKE_CURRENT_LIST_DIR}")
include_directories (${CMAKE_BINARY_DIR}) # need this because MOC outputs to binary_dir/pzp

add_definitions (-DSTD_LOF_BUILDING)

add_library (std-low-order-filters SHARED
    ${stdfilt_SOURCES}
    ${stdfilt_SOURCES_MOC}
    ${stdfilt_FORMS_MOC}
    ${stdfilt_RESOURCES_GEN}
    ${QM_FILES}
)

set_target_properties (std-low-order-filters PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
    PREFIX "")

add_dependencies (tfp std-low-order-filters)

###############################################################################
# dependencies
###############################################################################

target_link_libraries (std-low-order-filters Qt5::Core)
target_link_libraries (std-low-order-filters Qt5::Widgets)
target_link_libraries (std-low-order-filters Qt5::Gui)
target_link_libraries (std-low-order-filters Qt5::Concurrent)
target_link_libraries (std-low-order-filters Qt5::PrintSupport)
target_link_libraries (std-low-order-filters Qt5::OpenGL)
target_link_libraries (std-low-order-filters Qt5::Svg)

target_link_libraries (std-low-order-filters qwtinternal)
target_link_libraries (std-low-order-filters qwtplot3d)
target_link_libraries (std-low-order-filters qwtmathml)
target_link_libraries (std-low-order-filters kiss_fft)
target_link_libraries (std-low-order-filters pugixml)
