include (TFPPlugin)
add_plugin (dpsfg
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/DPSFGScene.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/DPSFGView.hpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ConnectionGraphicsObject.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/DPSFG.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/DPSFGScene.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/DPSFGView.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/NodeGraphicsObject.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Plugin.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Expression.cpp"
    TEST_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/tests/test_Graph.cpp"
)
