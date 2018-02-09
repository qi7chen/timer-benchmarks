// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "Benchmark.h"
#include "PQTimer.h"
#include "TreeTimer.h"
#include "WheelTimer.h"
#include "Clock.h"


inline void fillTimer(ITimerQueue* timer, std::vector<int>& vec, int n)
{
    auto dummy = []() {};
    for (int i = 0; i < n; i++)
    {
        int id = timer->AddTimer(i, dummy);
        vec.push_back(id);
    }
}

inline void benchCancel(ITimerQueue* timer, std::vector<int>& ids, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (i < (int)ids.size())
        {
            timer->CancelTimer(ids[i]);
        }
    }
}

inline void benchTick(ITimerQueue* timer, int n)
{
    for (int i = 0; i < n; i++)
    {
        timer->Update();
    }
}


BENCHMARK(TreeTimerAdd, n)
{
    TreeTimer timer;
    std::vector<int> ids;
    BENCHMARK_SUSPEND
    {
        ids.reserve(n);
    }
    fillTimer(&timer, ids, n);
}

BENCHMARK_RELATIVE(PQTimerAdd, n)
{
    PQTimer timer;
    std::vector<int> ids;
    BENCHMARK_SUSPEND
    {
        ids.reserve(n);
    }
    fillTimer(&timer, ids, n);
}

BENCHMARK_RELATIVE(WheelTimerAdd, n)
{
    WheelTimer timer;
    std::vector<int> ids;
    BENCHMARK_SUSPEND
    {
        ids.reserve(n);
    }
    fillTimer(&timer, ids, n);
}

BENCHMARK_DRAW_LINE()

BENCHMARK(TreeTimerDel, n)
{
    TreeTimer timer;
    std::vector<int> ids;
    BENCHMARK_SUSPEND
    {
        ids.reserve(n);
        fillTimer(&timer, ids, n);
    }
    benchCancel(&timer, ids, n);
}

BENCHMARK_RELATIVE(PQTimerDel, n)
{
    PQTimer timer;
    std::vector<int> ids;
    BENCHMARK_SUSPEND
    {
        ids.reserve(n);
        fillTimer(&timer, ids, n);
    }
    benchCancel(&timer, ids, n);
}

BENCHMARK_RELATIVE(WheelTimerDel, n)
{
    WheelTimer timer;
    std::vector<int> ids;
    BENCHMARK_SUSPEND
    {
        ids.reserve(n);
        fillTimer(&timer, ids, n);
    }
    benchCancel(&timer, ids, n);
}

BENCHMARK_DRAW_LINE()

BENCHMARK(TreeTimerTick, n)
{
    TreeTimer timer;
    std::vector<int> ids;
    BENCHMARK_SUSPEND
    {
        ids.reserve(n);
        fillTimer(&timer, ids, n);
    }
    benchTick(&timer, n);
}

BENCHMARK_RELATIVE(PQTimerTick, n)
{
    PQTimer timer;
    std::vector<int> ids;
    BENCHMARK_SUSPEND
    {
        ids.reserve(n);
        fillTimer(&timer, ids, n);
    }
    benchTick(&timer, n);
}

BENCHMARK_RELATIVE(WheelTimerTick, n)
{
    WheelTimer timer;
    std::vector<int> ids;
    BENCHMARK_SUSPEND
    {
        ids.reserve(n);
        fillTimer(&timer, ids, n);
    }
    benchTick(&timer, n);
}
