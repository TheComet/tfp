configure_file ("include/tfp/config.hpp.in" "include/tfp/config.hpp")

###############################################################################
# source files and library definition
###############################################################################

find_package (Qt5Widgets REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Core REQUIRED)
find_package (Qt5OpenGL REQUIRED)
find_package (Qt5LinguistTools REQUIRED)
find_package (FLEX REQUIRED)
find_package (BISON REQUIRED)

set (TFP_HEADERS_MOC
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/plot/DeltaPlotPicker.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/plot/Panner.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/plot/RectangleZoomer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/plot/Zoomer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/AdjustmentWidget.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/FloatAdjustmentWidget.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/InfiniSliderWidget.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/MainWindow.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/RealtimePlot.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/SettingsView.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/tfp/views/ToolContainer.hpp")
set (TFP_FORMS
    "${CMAKE_CURRENT_LIST_DIR}/forms/MainWindow.ui"
    "${CMAKE_CURRENT_LIST_DIR}/forms/SettingsView.ui")
set (TFP_RESOURCES
    "${CMAKE_CURRENT_LIST_DIR}/res/res.qrc")
set (TFP_TRANSLATION_FILES
    "")

set (TFP_test_SOURCES "")
if (TFP_BUILD_TESTS)
    set (TFP_test_SOURCES
        )
    add_executable (test_all
        "${CMAKE_CURRENT_LIST_DIR}/tests/run_tests_all.cpp")
    add_executable (test_tfp
        "${CMAKE_CURRENT_LIST_DIR}/tests/run_tests_tfp.cpp")
    target_link_libraries (test_all tfp)
    target_link_libraries (test_tfp tfp)
endif ()

qt5_wrap_cpp (TFP_SOURCES_MOC ${TFP_HEADERS_MOC})
qt5_wrap_ui  (TFP_FORMS_MOC ${TFP_FORMS})
qt5_add_resources (TFP_RESOURCES_GEN ${TFP_RESOURCES})
#qt5_add_translation (TFP_QM_FILES ${TFP_TRANSLATION_FILES})

# Expression parser
file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src/math/new")
file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/tfp/math/new")
bison_target (ExpressionParser
    "${CMAKE_CURRENT_LIST_DIR}/src/math/new/ExpressionParser.y"
    "${CMAKE_CURRENT_BINARY_DIR}/src/math/new/ExpressionParser.cpp"
    DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/tfp/math/new/ExpressionParser.hpp")
flex_target (ExpressionLexer
    "${CMAKE_CURRENT_LIST_DIR}/src/math/new/ExpressionLexer.l"
    "${CMAKE_CURRENT_BINARY_DIR}/src/math/new/ExpressionLexer.cpp")
add_flex_bison_dependency (ExpressionLexer ExpressionParser)

add_library (tfp ${TFP_LIB_TYPE}
    ${TFP_SOURCES_MOC}
    ${TFP_FORMS_MOC}
    ${TFP_RESOURCES_GEN}
    ${BISON_ExpressionParser_OUTPUTS}
    ${FLEX_ExpressionLexer_OUTPUTS}
    ${TFP_QM_FILES}
    "${CMAKE_CURRENT_LIST_DIR}/src/tfp.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/CoefficientPolynomial.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/Connection.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/Expression.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionParser.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionManipulator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionManipulator_RationalFunction.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionOptimiser.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionOptimiser_Constants.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionOptimiser_Simplifications.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/ExpressionOptimiser_UselessOperations.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/Graph.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/Math.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/Node.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/RootPolynomial.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/StateSpace.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/SymbolicMatrix.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/SymbolicStateSpace.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/System.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/TransferFunction.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/new/Expression.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/math/VariableTable.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/models/Config.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/models/Translation.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plot/AutoScaler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plot/ColourPalette.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plot/DeltaPlotPicker.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plot/MousePlotPickerMachine.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plot/Panner.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plot/RectangleZoomer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plot/UnitTracker.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plot/Zoomer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plugin/Plugin.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plugin/PluginManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/plugin/Tool.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/util/Logger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/util/Metric.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/util/RefCounted.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/util/String.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/util/Tears.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/util/Util.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/views/AdjustmentWidget.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/views/DataTreeView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/views/FloatAdjustmentWidget.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/views/InfiniSliderWidget.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/views/MainWindow.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/views/RealtimePlot.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/views/RealtimePlot3D.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/views/SettingsView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/views/ToolContainer.cpp"
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/run_tests_api.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_Expression.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_ExpressionManipulator_RationalFunction.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_ExpressionOptimiser_Constants.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_ExpressionOptimiser_Simplifications.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_ExpressionOptimiser_UselessOperations.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_Expression_parser.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_Graph.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_Node.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_StateSpace.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_SymbolicMatrix.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_SymbolicStateSpace.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/test_VariableTable.cpp">
    $<$<BOOL:${TFP_TESTS}>:"${CMAKE_CURRENT_LIST_DIR}/tests/new/test_Expression.cpp">)
target_include_directories(tfp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
target_compile_options (tfp
    PRIVATE
        $<$<COMPILER_ID:GNU>:-W -Wall -Wextra -pedantic>
        $<$<COMPILER_ID:Clang>:-W -Wall -Wextra -pedantic>)
target_compile_definitions (tfp
    PRIVATE
        TFP_BUILDING)
target_link_libraries (tfp
    PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        qwtinternal
        qwtplot3d
        ads
        kissfft
        pugixml
        $<$<BOOL:${TFP_TESTS}>:gtest>)

#add_custom_target (TFP_translations_compile
#    COMMAND ${LRELEASE_COMMAND} ${CMAKE_CURRENT_LIST_DIR}/translations/TFP_de.ts
#    COMMENT "Compiling translations"
#    VERBATIM
#)
#add_custom_target (TFP_translations_move
#    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations
#    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_LIST_DIR}/translations/TFP_de.qm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/TFP_de.qm
#    VERBATIM
#)
#add_dependencies (TFP_translations_move TFP_translations_compile)
#add_dependencies (tfp TFP_translations_move)

###############################################################################
# DLLs need to be copied on Windows
###############################################################################

if (PLATFORM MATCHES "WINDOWS")
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        add_custom_target (Qt5CopyBinaries
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../plugins/platforms/qwindowsd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Cored.dll         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Guid.dll          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5OpenGLd.dll       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5PrintSupportd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Svgd.dll          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Widgetsd.dll      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libGLESv2d.dll       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libEGLd.dll          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

            COMMENT "Copying Qt binaries from '${Qt5Core_DIR}/../../bin/' to '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'"
            VERBATIM
        )
        add_dependencies (tfp Qt5CopyBinaries)
    endif ()
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        add_custom_target (Qt5CopyBinaries
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../plugins/platforms/qwindows.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Core.dll          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Gui.dll           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5OpenGL.dll        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5PrintSupport.dll  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Svg.dll           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Widgets.dll       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libGLESv2.dll        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libEGL.dll           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

            COMMENT "Copying Qt binaries from '${Qt5Core_DIR}/../../bin/' to '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'"
            VERBATIM
        )
        add_dependencies (tfp Qt5CopyBinaries)
    endif ()
endif ()
