###############################################################################
# source files and library definition
###############################################################################

set (sir_HEADERS_MOC "")

set (sir_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/TimeDomainResponsePlot.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ImpulsePlot.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/StepPlot.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Plugin.cpp")
set (sir_FORMS "")
set (sir_RESOURCES "")

set (sir_TRANSLATION_FILES "")

qt5_wrap_cpp (sir_SOURCES_MOC ${sir_HEADERS_MOC})
qt5_wrap_ui  (sir_FORMS_MOC ${sir_FORMS})
qt5_add_resources (sir_RESOURCES_GEN ${sir_RESOURCES})
#qt5_add_translation (sir_QM_FILES ${sir_TRANSLATION_FILES})

#include_directories ("${CMAKE_CURRENT_LIST_DIR}")
include_directories (${CMAKE_BINARY_DIR}) # need this because MOC outputs to binary_dir/pzp

add_definitions (-DSE_BUILDING)

add_library (step-impulse-response SHARED
    ${sir_SOURCES}
    ${sir_SOURCES_MOC}
    ${sir_FORMS_MOC}
    ${sir_RESOURCES_GEN}
    ${QM_FILES}
)

set_target_properties (step-impulse-response PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
    PREFIX "")

add_dependencies (tfp step-impulse-response)

###############################################################################
# dependencies
###############################################################################

target_link_libraries (step-impulse-response Qt5::Core)
target_link_libraries (step-impulse-response Qt5::Widgets)
target_link_libraries (step-impulse-response Qt5::Gui)
target_link_libraries (step-impulse-response Qt5::Concurrent)
target_link_libraries (step-impulse-response Qt5::PrintSupport)
target_link_libraries (step-impulse-response Qt5::OpenGL)
target_link_libraries (step-impulse-response Qt5::Svg)

target_link_libraries (step-impulse-response qwtinternal)
target_link_libraries (step-impulse-response qwtplot3d)
target_link_libraries (step-impulse-response qwtmathml)
target_link_libraries (step-impulse-response kiss_fft)
target_link_libraries (step-impulse-response pugixml)
