#include <iostream>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "tfp/config.hpp"
#include "tfp/plugin/PluginManager.hpp"
#include "tfp/views/DataTree.hpp"
#include <QApplication>

namespace tfp {

TFP_PUBLIC_API int run_tests(int argc, char** argv)
{
    std::cout << "Running tests from run_tests() in tfp library\n";
    // Since Google Mock depends on Google Test, InitGoogleMock() is
    // also responsible for initializing Google Test.  Therefore there's
    // no need for calling testing::InitGoogleTest() separately.
    testing::InitGoogleMock(&argc, argv);
    int thisResult = RUN_ALL_TESTS();

    QApplication app(argc, argv);
    PluginManager* pm = new PluginManager(new DataTree);
    pm->loadAllPlugins();
    QVector<QString> failedPluginTests = pm->runPluginTests(argc, argv);
    delete pm;

    std::cout << "----------------------------------------------------------" << std::endl;
    if (thisResult != 0 || failedPluginTests.size() != 0)
        std::cout << "Unit tests failed in the following libraries!" << std::endl;
    else
        std::cout << "All unit tests passed!" << std::endl;
    for (QVector<QString>::const_iterator it = failedPluginTests.begin(); it != failedPluginTests.end(); ++it)
        std::cout << "  - plugin \"" << it->toStdString() << "\"" << std::endl;
    if (thisResult != 0)
        std::cout << "  - tfp library" << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    if (thisResult != 0 || failedPluginTests.size() != 0)
        return -1;
    return 0;
}

}
