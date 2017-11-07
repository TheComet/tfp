include (TFPPlugin)
add_plugin (dpsfg
    INCLUDE_DIRS
        "${CMAKE_CURRENT_LIST_DIR}"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/view/DPSFGScene.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/view/DPSFGView.hpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Plugin.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/view/ConnectionGraphicsObject.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/view/DPSFG.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/view/DPSFGScene.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/view/DPSFGView.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/view/NodeGraphicsObject.cpp"
)
