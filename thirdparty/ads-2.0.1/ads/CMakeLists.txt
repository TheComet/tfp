if (ADS_SHARED_LIB)
    set (BUILD_TYPE SHARED)
else ()
    set (BUILD_TYPE STATIC)
endif ()

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

find_package (Qt5 COMPONENTS Core Gui Widgets REQUIRED)
qt5_wrap_cpp (ADS_MOC_SOURCES ${ADS_MOC_HEADERS})
qt5_add_resources (ADS_RESOURCES_GEN ${ADS_RESOURCES})

include_directories (${ADS_INCLUDE_DIRS})

add_library (ads ${BUILD_TYPE}
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
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets)

###############################################################################
# install targets
###############################################################################

install (
    TARGETS ads
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)
