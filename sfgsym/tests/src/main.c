#include "sfgsym/tests.h"
#include "sfgsym/init.h"

int main(int argc, char** argv)
{
    int result;

    if (sfgsym_init() != 0)
        return -1;

    result = sfgsym_run_tests(argc, argv);
    sfgsym_deinit();
    return result;
}

