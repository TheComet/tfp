#include <gmock/gmock.h>
#include "sfgsym/tests.h"

/* ------------------------------------------------------------------------- */
int sfgsym_run_tests(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

