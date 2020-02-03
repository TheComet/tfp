include (TestVisibilityMacros)
include (GNUInstallDirs)

test_visibility_macros (ADS_API_EXPORT ADS_API_IMPORT ADS_API_LOCAL)

configure_file ("include/ads/config.h.in" "include/ads/config.h")

###############################################################################
# Library, source and header files
###############################################################################

set (ADS_HEADERS
    "include/ads/globals.h"
    "include/ads/DockAreaWidget.h"
    "include/ads/DockContainerWidget.h"
    "include/ads/DockManager.h"
    "include/ads/DockWidget.h"
    "include/ads/DockWidgetTitleBar.h"
    "include/ads/FloatingDockContainer.h"
    "include/ads/DockOverlay.h"
    "include/ads/DockSplitter.h")
set (ADS_SOURCES
    "src/globals.cpp"
    "src/DockAreaWidget.cpp"
    "src/DockContainerWidget.cpp"
    "src/DockManager.cpp"
    "src/DockWidget.cpp"
    "src/DockWidgetTitleBar.cpp"
    "src/FloatingDockContainer.cpp"
    "src/DockOverlay.cpp"
    "src/DockSplitter.cpp")
set (ADS_MOC_HEADERS
    "include/ads/DockAreaWidget.h"
    "include/ads/DockContainerWidget.h"
    "include/ads/DockManager.h"
    "include/ads/DockWidget.h"
    "include/ads/DockWidgetTitleBar.h"
    "include/ads/FloatingDockContainer.h"
    "include/ads/DockOverlay.h"
    "include/ads/DockSplitter.h")
set (ADS_RESOURCES
    "res/ads.qrc")

find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Widgets REQUIRED)

qt5_wrap_cpp (ADS_MOC_SOURCES ${ADS_MOC_HEADERS})
qt5_add_resources (ADS_RESOURCES_GEN ${ADS_RESOURCES})

add_library (ads ${ADS_LIB_TYPE}
    ${ADS_HEADERS}
    ${ADS_SOURCES}
    ${ADS_MOC_SOURCES}
    ${ADS_RESOURCES_GEN})
target_compile_options (ads
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-W -Wall -Wextra -pedantic -Wno-unused-parameter>
        $<$<CXX_COMPILER_ID:Clang>:-W -Wall -Wextra -pedantic -Wno-unused-parameter>)
target_compile_definitions (ads
    PRIVATE
        ADS_EXPORT
        ADS_BUILDING)
target_include_directories(ads
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_link_libraries (ads
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets)

###############################################################################
# install targets
###############################################################################

if (${ADS_LIB_TYPE} MATCHES "SHARED")
    install (
        TARGETS ads
        LIBRARY DESTINATION "lib" COMPONENT runtime
        RUNTIME DESTINATION "bin" COMPONENT runtime)
endif ()

if (${INSTALL_DEV})
    install (
        TARGETS ads
        EXPORT ADSConfig
        ARCHIVE DESTINATION "lib"
        LIBRARY DESTINATION "lib"
        RUNTIME DESTINATION "bin")
    install (
        DIRECTORY "include/ads"
        DESTINATION "include"
        FILES_MATCHING PATTERN "*.h")
    install (
        DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/ads"
        DESTINATION "include"
        FILES_MATCHING PATTERN "*.h")
    install (
        EXPORT ADSConfig
        DESTINATION "share/ADS/cmake")
    export (
        TARGETS ads
        FILE ADSConfig.cmake)
endif ()
