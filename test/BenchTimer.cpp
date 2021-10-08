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

BENCHMARK(PQTimerAdd , n)
{
    PQTimer timer;
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
    }

    fillTimer(&timer, ids, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(TreeTimerAdd, n)
{
    TreeTimer timer;
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
    }

    fillTimer(&timer, ids, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(WheelTimerAdd, n)
{
    WheelTimer timer;
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
    }

    fillTimer(&timer, ids, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_DRAW_LINE()


BENCHMARK(PQTimerDel, n)
{
    PQTimer timer;
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(&timer, ids, MaxN);
    }

    benchCancel(&timer, ids);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(TreeTimerDel, n)
{
    TreeTimer timer;
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(&timer, ids, MaxN);
    }

    benchCancel(&timer, ids);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(WheelTimerDel, n)
{
    WheelTimer timer;
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(&timer, ids, MaxN);
    }

    benchCancel(&timer, ids);

    doNotOptimizeAway(timer);
}

BENCHMARK_DRAW_LINE()


BENCHMARK(PQTimerTick, n)
{
    PQTimer timer;
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(&timer, ids, MaxN);
    }

    benchTick(&timer, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(TreeTimerTick, n)
{
    TreeTimer timer;
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(&timer, ids, MaxN);
    }

    benchTick(&timer, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(WheelTimerTick, n)
{
    WheelTimer timer;
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(&timer, ids, MaxN);
    }

    benchTick(&timer, MaxN);

    doNotOptimizeAway(timer);
}
