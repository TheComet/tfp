#include "benchmark/benchmark.h"
#include "cstructures/init.h"

using namespace benchmark;

int main(int argc, char** argv) {
    cstructures_init();
    Initialize(&argc, argv);
    if (ReportUnrecognizedArguments(argc, argv))
        return 1;
    RunSpecifiedBenchmarks();
    cstructures_deinit();
}
