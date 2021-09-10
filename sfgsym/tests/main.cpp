#include <gmock/gmock.h>
#include "cstructures/init.h"

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    cstructures_init();
    int result = RUN_ALL_TESTS();
    cstructures_deinit();
    return result;
}

