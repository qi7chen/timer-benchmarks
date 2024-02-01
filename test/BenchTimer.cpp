// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include <algorithm>
#include "TimerBase.h"
#include "Clock.h"
#include "Preprocessor.h"
#include <benchmark/benchmark.h>
#include <vector>

using namespace std;

const int MaxN = 50000;   // max timer count

// see https://en.wikipedia.org/wiki/Linear_congruential_generator
uint32_t lcg_seed(uint32_t seed) {
    return seed * 214013 + 2531011;
}

uint32_t lcg_rand(uint32_t& seed) {
    seed = seed * 214013 + 2531011;
    uint32_t r = uint32_t(seed >> 16) & 0x7fff;
    return r;
}

static std::shared_ptr<TimerBase> createAndStartTimer(TimerSchedType timerType, benchmark::State& state) {
    uint32_t seed = lcg_seed(12345);

    auto timer = CreateTimer(timerType);
    auto dummy = []() {};
    for (auto _ : state)
    {
        uint32_t duration = lcg_rand(seed) % 5000;
        timer->Start(duration, dummy);
    }
    return timer;
}

static void BM_PQTimerAdd(benchmark::State& state)
{
    auto timer = createAndStartTimer(TimerSchedType::TIMER_PRIORITY_QUEUE, state);
    doNotOptimizeAway(timer);
}

static void BM_QuadHeapTimerAdd(benchmark::State& state)
{
    auto timer = createAndStartTimer(TimerSchedType::TIMER_QUAD_HEAP, state);
    doNotOptimizeAway(timer);
}

static void BM_RBTreeTimerAdd(benchmark::State& state)
{
    auto timer = createAndStartTimer(TimerSchedType::TIMER_RBTREE, state);
    doNotOptimizeAway(timer);
}

static void BM_HashWheelTimerAdd(benchmark::State& state)
{
    auto timer = createAndStartTimer(TimerSchedType::TIMER_HASHED_WHEEL, state);
    doNotOptimizeAway(timer);
}

static void BM_HHWheelTimerAdd(benchmark::State& state)
{
    auto timer = createAndStartTimer(TimerSchedType::TIMER_HH_WHEEL, state);
    doNotOptimizeAway(timer);
}

BENCHMARK(BM_PQTimerAdd);
BENCHMARK(BM_QuadHeapTimerAdd);
BENCHMARK(BM_RBTreeTimerAdd);
BENCHMARK(BM_HashWheelTimerAdd);
BENCHMARK(BM_HHWheelTimerAdd);


static std::shared_ptr<TimerBase> createAndFillTimer(TimerSchedType timerType, int N, vector<int>& out) {
    uint32_t seed = lcg_seed(12345);
    auto timer = CreateTimer(timerType);
    auto dummy = []() {};
    for (int i = 0; i < N; i++)
    {
        uint32_t duration = lcg_rand(seed) % 5000;
        int tid = timer->Start(duration, dummy);
        out.push_back(tid);
    }
    std::random_shuffle(out.begin(), out.end());
    return timer;
}

static void benchTimerCancel(TimerSchedType timerType, benchmark::State& state)
{
    int N = (int)state.max_iterations;
    vector<int> timer_ids;
    timer_ids.reserve(N);
    auto timer = createAndFillTimer(timerType, N, timer_ids);
    for (auto _ : state)
    {
        if (timer_ids.empty()) {
            break;
        }
        int timer_id = timer_ids.back();
        timer_ids.pop_back();
        timer->Cancel(timer_id);
    }
    doNotOptimizeAway(timer);
}

static void BM_PQTimerCancel(benchmark::State& state) {

    benchTimerCancel(TimerSchedType::TIMER_PRIORITY_QUEUE, state);
}

static void BM_QuadHeapTimerCancel(benchmark::State& state) {

    benchTimerCancel(TimerSchedType::TIMER_QUAD_HEAP, state);
}

static void BM_RBTreeTimerCancel(benchmark::State& state) {

    benchTimerCancel(TimerSchedType::TIMER_RBTREE, state);
}

static void BM_HashWheelTimerCancel(benchmark::State& state) {

    benchTimerCancel(TimerSchedType::TIMER_HASHED_WHEEL, state);
}

static void BM_HHWheelTimerCancel(benchmark::State& state) {

    benchTimerCancel(TimerSchedType::TIMER_HH_WHEEL, state);
}


BENCHMARK(BM_PQTimerCancel);
BENCHMARK(BM_QuadHeapTimerCancel); // lazy deletion here not fair
BENCHMARK(BM_RBTreeTimerCancel);
BENCHMARK(BM_HashWheelTimerCancel);
BENCHMARK(BM_HHWheelTimerCancel);


static void benchTimerTick(TimerSchedType timerType, benchmark::State& state)
{
    vector<int> timer_ids;
    timer_ids.reserve(MaxN);
    auto timer = createAndFillTimer(timerType, MaxN, timer_ids);
    for (auto _ : state)
    {
        timer->Update(Clock::CurrentTimeMillis());
    }
    doNotOptimizeAway(timer);
}

static void BM_PQTimerTick(benchmark::State& state) {

    benchTimerTick(TimerSchedType::TIMER_PRIORITY_QUEUE, state);
}

static void BM_QuadHeapTimerTick(benchmark::State& state) {

    benchTimerTick(TimerSchedType::TIMER_QUAD_HEAP, state);
}

static void BM_RBTreeTimerTick(benchmark::State& state) {

    benchTimerTick(TimerSchedType::TIMER_RBTREE, state);
}

static void BM_HashWheelTimerTick(benchmark::State& state) {

    benchTimerTick(TimerSchedType::TIMER_HASHED_WHEEL, state);
}

static void BM_HHWheelTimerTick(benchmark::State& state) {

    benchTimerTick(TimerSchedType::TIMER_HH_WHEEL, state);
}


BENCHMARK(BM_PQTimerTick);
BENCHMARK(BM_QuadHeapTimerTick);
BENCHMARK(BM_RBTreeTimerTick);
BENCHMARK(BM_HashWheelTimerTick);
BENCHMARK(BM_HHWheelTimerTick);

