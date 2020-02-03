###############################################################################
# Library, source and header files
###############################################################################

set (ADS_DEMO_HEADERS
    "mainwindow.h")
set (ADS_DEMO_SOURCES
    "mainwindow.cpp"
    "main.cpp")
set (ADS_DEMO_MOC_HEADERS
    "mainwindow.h")
set (ADS_DEMO_FORMS
    "mainwindow.ui")

find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Widgets REQUIRED)

qt5_wrap_cpp (ADS_DEMO_MOC_SOURCES ${ADS_DEMO_MOC_HEADERS})
qt5_wrap_ui (ADS_DEMO_FORMS_MOC ${ADS_DEMO_FORMS})

add_executable (ads_demo
    ${ADS_DEMO_HEADERS}
    ${ADS_DEMO_SOURCES}
    ${ADS_DEMO_MOC_SOURCES}
    ${ADS_DEMO_FORMS_MOC})
target_link_libraries (ads_demo
    PRIVATE
        ads)
target_include_directories (ads_demo
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE $<INSTALL_INTERFACE:install>)
set_target_properties (ads_demo
    PROPERTIES
        INSTALL_RPATH "../lib")

###############################################################################
# install targets
###############################################################################

install (
    TARGETS ads_demo
    RUNTIME DESTINATION "bin")
