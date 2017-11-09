#include "tfp/tfp.hpp"
#include "tfp/util/Logger.hpp"
#include "tfp/models/Config.hpp"

#ifdef TFP_BUILD_TESTS
#include <gmock/gmock.h>
#endif

using namespace tfp;

extern "C" {

void tfp_init(int* argc, char** argv)
{
#ifdef TFP_BUILD_TESTS
    // Since Google Mock depends on Google Test, InitGoogleMock() is
    // also responsible for initializing Google Test.  Therefore there's
    // no need for calling testing::InitGoogleTest() separately.
    testing::InitGoogleMock(argc, argv);
#endif

    // Try to open log in executable directory. If that fails, open the log
    // in the current working directory instead
    if (Logger::openDefaultLog(*argc, argv) == false)
        Logger::openDefaultLog();

    // QApplication needs to have been instantiated before loading this,
    // because the config relies on QtCore calls that are otherwise not
    // initialized.
    g_config.load();
}

void tfp_deinit()
{
#ifdef TFP_BUILD_TESTS
    // Removes all unit tests that are globally registered.
    CLEAR_ALL_TESTS();
#endif
}

}
