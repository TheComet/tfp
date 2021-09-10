#include "benchmark/benchmark.h"
#include "cstructures/hashmap.h"
#include "cstructures/hash.h"
#include <iostream>
#include <random>
#include <unordered_map>
#include <string.h>
#include <vector>

using namespace benchmark;

static std::mt19937 rng;

static uint8_t randomChar()
{
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    return dist(rng);
}

static void fillRandom(char* s, uint32_t len)
{
    while (len--)
        *s++ = randomChar();
}

static void BM_HashmapCreation(State& state)
{
    uint32_t key_size = state.range(0);
    uint32_t value_size = state.range(1);

    for (auto _ : state)
    {
        hashmap_t hm;
        hashmap_init(&hm, key_size, value_size);
        DoNotOptimize(hm.storage);
        hashmap_deinit(&hm);
    }
}
BENCHMARK(BM_HashmapCreation)
    //->RangeMultiplier(2)->Ranges({{1<<0, 1<<16}, {1<<0, 1<<16}})
    ->RangeMultiplier(64)->Ranges({{1<<4, 1<<8}, {1<<4, 1<<8}})
    ;

static void BM_HashmapInsert(State& state)
{
    int insertions = state.range(0);
    int keySize = state.range(1);
    int valueSize = state.range(2);
    std::vector<std::vector<char>> keys(insertions, std::vector<char>(keySize));
    std::vector<std::vector<char>> values(insertions, std::vector<char>(valueSize));
    for (auto& key : keys)
        fillRandom(key.data(), keySize);
    for (auto& value : values)
        fillRandom(value.data(), valueSize);

    for (auto _ : state)
    {
        hashmap_t hm;
        hashmap_init(&hm, keySize, valueSize);
        DoNotOptimize(hm.storage);
        for (int i = 0; i != state.range(0); ++i)
            hashmap_insert(&hm, keys[i].data(), values[i].data());
        ClobberMemory();
        hashmap_deinit(&hm);
    }
}

BENCHMARK(BM_HashmapInsert)->RangeMultiplier(2)->Ranges({{1<<0, 1<<16}, {1<<0, 1<<10}, {1<<0, 1<<16}});
