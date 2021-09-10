#include "benchmark/benchmark.h"
#include <vector>

using namespace benchmark;

static void BM_StdVectorCreation(State& state)
{
    for (auto _ : state)
    {
        std::vector<int> v;
        DoNotOptimize(v.size());
    }
}
BENCHMARK(BM_StdVectorCreation);

static void BM_StdVectorResize(State& state)
{
    for (auto _ : state)
    {
        std::vector<int> v;
        v.reserve(4);
        DoNotOptimize(v.data());
    }
}
BENCHMARK(BM_StdVectorResize);

static void BM_StdVectorReservePush(State& state)
{
    int someInt = 4;
    for (auto _ : state)
    {
        std::vector<int> v;
        v.reserve(4);
        DoNotOptimize(v.data());
        v.push_back(someInt);
        ClobberMemory();
    }
}
BENCHMARK(BM_StdVectorReservePush);
