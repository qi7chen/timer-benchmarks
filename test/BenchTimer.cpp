// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "Benchmark.h"
#include "PQTimer.h"
#include "TreeTimer.h"
#include "WheelTimer.h"
#include "Clock.h"

const int MaxN = 1000000;   // max node count

inline void fillTimer(TimerQueueBase* timer, std::vector<int>& vec, int n)
{
    auto dummy = []() {};
    for (int i = 0; i < n; i++)
    {
        int id = timer->AddTimer(i, dummy);
        vec.push_back(id);
    }
}

inline void benchCancel(TimerQueueBase* timer, std::vector<int>& ids, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (i < (int)ids.size())
        {
            timer->CancelTimer(ids[i]);
        }
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

    benchCancel(&timer, ids, MaxN);

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

    benchCancel(&timer, ids, MaxN);

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

    benchCancel(&timer, ids, MaxN);

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
