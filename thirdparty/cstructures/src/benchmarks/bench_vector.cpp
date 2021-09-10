#include "benchmark/benchmark.h"
#include "cstructures/vector.h"

using namespace benchmark;

static void BM_VectorCreation(State& state)
{
    for (auto _ : state)
    {
        vector_t v;
        vector_init(&v, sizeof(int));
        DoNotOptimize(&v);
        vector_deinit(&v);
    }
}
BENCHMARK(BM_VectorCreation);

static void BM_VectorReserve(State& state)
{
    for (auto _ : state)
    {
        vector_t v;
        vector_init(&v, sizeof(int));
        vector_reserve(&v, 1);
        DoNotOptimize(vector_data(&v));
        vector_deinit(&v);
    }
}
BENCHMARK(BM_VectorReserve);

static void BM_VectorReservePush(State& state)
{
    int someInt = 4;
    for (auto _ : state)
    {
        vector_t v;
        vector_init(&v, sizeof(int));
        vector_reserve(&v, 1);
        DoNotOptimize(vector_data(&v));
        vector_push(&v, &someInt);
        ClobberMemory();
        vector_deinit(&v);
    }
}
BENCHMARK(BM_VectorReservePush);
