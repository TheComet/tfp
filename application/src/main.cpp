#include "tfp/views/MainWindow.hpp"
#include "tfp/util/Logger.hpp"
#include "tfp/models/Config.hpp"
#include <iostream>
#include <QApplication>


// ----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    if (tfp::Logger::openDefaultLog(argc, argv) == false)
        tfp::Logger::openDefaultLog();

    QApplication application(argc, argv);
    tfp::MainWindow mainWindow;
    mainWindow.show();

    tfp::g_config.load();

    tfp::g_log.logNotice("Application initialised, entering main event loop");
    return application.exec();
}
