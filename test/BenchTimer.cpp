// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include <algorithm>
#include "TimerBase.h"
#include "Clock.h"
#include "Benchmark.h"
#include <vector>

using namespace std;

const int MaxN = 50000;   // max node count

// Add timer with random duration 
inline void fillTimer(std::shared_ptr<TimerBase> timer, std::vector<int>& ids, int n)
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
        int id = timer->Start(durations[i], dummy);
        ids.push_back(id);
    }
}

// Cancel timers with random timer id
inline void benchCancel(std::shared_ptr<TimerBase> timer, std::vector<int>& ids)
{
    std::random_shuffle(ids.begin(), ids.end());
    for (int i = 0; i < (int)ids.size(); i++)
    {
        timer->Cancel(ids[i]);
    }
}

inline void benchTick(std::shared_ptr<TimerBase> timer, int n)
{
    for (int i = 0; i < n; i++)
    {
        timer->Tick(Clock::CurrentTimeMillis());
    }
}


BENCHMARK(PQTimerAdd, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_PRIORITY_QUEUE);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
    }

    fillTimer(timer, ids, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(QuadHeapTimerAdd, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_RBTREE);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
    }

    fillTimer(timer, ids, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(RBTreeTimerAdd, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_RBTREE);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
    }

    fillTimer(timer, ids, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(HashedWheelTimerAdd, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_HASHED_WHEEL);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
    }

    fillTimer(timer, ids, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(HHWheelTimerAdd, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_HH_WHEEL);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
    }

    fillTimer(timer, ids, MaxN);

    doNotOptimizeAway(timer);
}


BENCHMARK_DRAW_LINE();


BENCHMARK(PQTimerDel, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_PRIORITY_QUEUE);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchCancel(timer, ids);

    doNotOptimizeAway(timer);
}

BENCHMARK(QuadHeapTimerDel, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_QUAD_HEAP);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchCancel(timer, ids);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(RBTreeTimerDel, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_RBTREE);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchCancel(timer, ids);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(HashedWheelTimerDel, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_HASHED_WHEEL);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchCancel(timer, ids);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(HHWheelTimerDel, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_HH_WHEEL);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchCancel(timer, ids);

    doNotOptimizeAway(timer);
}


BENCHMARK_DRAW_LINE();


BENCHMARK(PQTimerTick, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_PRIORITY_QUEUE);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchTick(timer, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK(QuadHeapTimerTick, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_QUAD_HEAP);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchTick(timer, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(RBTreeTimerTick, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_QUAD_HEAP);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchTick(timer, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(HashedWheelTimerTick, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_HASHED_WHEEL);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchTick(timer, MaxN);

    doNotOptimizeAway(timer);
}

BENCHMARK_RELATIVE(HHWheelTimerTick, n)
{
    auto timer = CreateTimer(TimerSchedType::TIMER_HH_WHEEL);
    std::vector<int> ids;

    BENCHMARK_SUSPEND
    {
        ids.reserve(MaxN);
        fillTimer(timer, ids, MaxN);
    }

    benchTick(timer, MaxN);

    doNotOptimizeAway(timer);
}
