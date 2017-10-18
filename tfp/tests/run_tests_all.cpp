#include <iostream>

#include "tfp/config.hpp"

namespace tfp {
TFP_PUBLIC_API int run_tests_all(int argc, char** argv);
}

int main(int argc, char** argv)
{
    return tfp::run_tests_all(argc, argv);
}
