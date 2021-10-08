// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.


#include <algorithm>
#include "PQTimer.h"
#include "TreeTimer.h"
#include "WheelTimer.h"
#include "Clock.h"
#include <benchmark/benchmark.h>


const int MaxN = 50000;   // max node count

// Add timer with random duration 
inline void fillTimer(TimerQueueBase* timer, std::vector<int>& ids, int n)
{
    std::vector<int> durations;
    for (int i = 0; i < n; i++)
    {
        durations.push_back(i);
    }
    std::random_shuffle(durations.begin(), durations.end());
    auto dummy = []() {};
    for (int i = 0; i < (int)durations.size(); i++)
    {
        int id = timer->Schedule(durations[i], dummy);
        ids.push_back(id);
    }
}

// Cancel timers with random timer id
inline void benchCancel(TimerQueueBase* timer, std::vector<int>& ids)
{
    std::random_shuffle(ids.begin(), ids.end());
    for (int i = 0; i < (int)ids.size(); i++)
    {
        timer->Cancel(ids[i]);
    }
}

inline void benchTick(TimerQueueBase* timer, int n)
{
    for (int i = 0; i < n; i++)
    {
        timer->Update();
    }
}

static void BM_PQTimerAdd(benchmark::State& state)
{
    PQTimer timer;
    std::vector<int> ids;

    state.PauseTiming();
    ids.reserve(MaxN);
    state.ResumeTiming();

    fillTimer(&timer, ids, MaxN);

    DoNotOptimize(timer);
}

BENCHMARK(BM_PQTimerAdd);

static void BM_TreeTimerAdd(benchmark::State& state)
{
    TreeTimer timer;
    std::vector<int> ids;

    state.PauseTiming();
    ids.reserve(MaxN);
    state.ResumeTiming();

    fillTimer(&timer, ids, MaxN);

    DoNotOptimize(timer);
}

BENCHMARK(BM_TreeTimerAdd);


static void BM_WheelTimerAdd(benchmark::State& state)
{
    WheelTimer timer;
    std::vector<int> ids;

    state.PauseTiming();
    ids.reserve(MaxN);
    state.ResumeTiming();

    fillTimer(&timer, ids, MaxN);

    DoNotOptimize(timer);
}

BENCHMARK(BM_WheelTimerAdd);


static void BM_PQTimerDel(benchmark::State& state)
{
    PQTimer timer;
    std::vector<int> ids;

    state.PauseTiming();
    ids.reserve(MaxN);
    fillTimer(&timer, ids, MaxN);
    state.ResumeTiming();

    benchCancel(&timer, ids);

    DoNotOptimize(timer);
}

BENCHMARK(BM_PQTimerDel);

static void BM_TreeTimerDel(benchmark::State& state)
{
    TreeTimer timer;
    std::vector<int> ids;

    state.PauseTiming();
    ids.reserve(MaxN);
    fillTimer(&timer, ids, MaxN);
    state.ResumeTiming();

    benchCancel(&timer, ids);

    DoNotOptimize(timer);
}

BENCHMARK(BM_TreeTimerDel);

static void BM_WheelTimerDel(benchmark::State& state)
{
    WheelTimer timer;
    std::vector<int> ids;

    state.PauseTiming();
    ids.reserve(MaxN);
    fillTimer(&timer, ids, MaxN);
    state.ResumeTiming();

    benchCancel(&timer, ids);

    DoNotOptimize(timer);
}

BENCHMARK(BM_WheelTimerDel);


static void BM_PQTimerTick(benchmark::State& state)
{
    PQTimer timer;
    std::vector<int> ids;

    state.PauseTiming();
    ids.reserve(MaxN);
    fillTimer(&timer, ids, MaxN);
    state.ResumeTiming();

    benchTick(&timer, MaxN);

    DoNotOptimize(timer);
}

BENCHMARK(BM_PQTimerTick);

static void BM_TreeTimerTick(benchmark::State& state)
{
    TreeTimer timer;
    std::vector<int> ids;

    state.PauseTiming();
    ids.reserve(MaxN);
    fillTimer(&timer, ids, MaxN);
    state.ResumeTiming();

    benchTick(&timer, MaxN);

    DoNotOptimize(timer);
}

static void BM_WheelTimerTick(benchmark::State& state)
{
    WheelTimer timer;
    std::vector<int> ids;

    state.PauseTiming();
    ids.reserve(MaxN);
    fillTimer(&timer, ids, MaxN);
    state.ResumeTiming();

    benchTick(&timer, MaxN);

    DoNotOptimize(timer);
}
