// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include <chrono>
#include <thread>
#include <numeric>
#include <algorithm>
#include <gtest/gtest.h>
#include "PQTimer.h"
#include "TreeTimer.h"
#include "WheelTimer.h"
#include "Clock.h"
#include "Benchmark.h"

static int N1 = 2000;
static int N2 = 10;
static int TRY = 2;

struct timerContext
{
    int64_t schedule = 0;   // when timer schedule to run
    int64_t fired = 0;      // when timer fired
    int interval = 0;       // interval milliseconds
};

static void TestTimerAdd(TimerQueueBase* timer, int count)
{
    int called = 0;
    auto callback = [&]()
    {
        called++;
    };
    for (int i = 0; i < count; i++)
    {
        timer->RunAfter(0, callback);
    }
    EXPECT_EQ(timer->Size(), count);
    int fired = timer->Update();
    EXPECT_EQ(fired, count);
    EXPECT_EQ(called, count);
    EXPECT_EQ(timer->Size(), 0);
    called = 0;

    for (int i = 0; i < count; i++)
    {
        int id = timer->RunAfter(0, callback);
        timer->Cancel(id);
    }
    fired = timer->Update();
    EXPECT_EQ(fired, 0);
    EXPECT_EQ(timer->Size(), 0);
    EXPECT_EQ(called, 0);

    doNotOptimizeAway(called);
    doNotOptimizeAway(fired);
}

static void TestTimerExpire(TimerQueueBase* timer, int count)
{
    std::vector<timerContext> fired_records;
    std::vector<TimerCallback> timer_callbacks;
    fired_records.resize(count);
    timer_callbacks.resize(count);

    const int TIME_DELTA = 10;

    for (int i = 0; i < count; i++)
    {
        auto fn = [&](int idx)
        {
            fired_records[idx].fired = Clock::CurrentTimeMillis(); // when timer fired
        };
        timer_callbacks[i] = std::bind(fn, i);
        int interval = 1000 + i * TIME_DELTA;
        fired_records[i].interval = interval;
        fired_records[i].schedule = Clock::CurrentTimeMillis(); // when timer scheduled
        timer->RunAfter(interval, timer_callbacks[i]);

        // this is to avoid all timers started at same time
        int sleep_interval = rand() % TIME_DELTA;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_interval));
        
    }
    EXPECT_EQ(timer->Size(), count);

    int fired = 0;
    while (fired < count)
    {
        fired += timer->Update();
    }

    EXPECT_EQ(timer->Size(), 0);
    
    std::vector<int> interval_tolerance;
    interval_tolerance.reserve(count);

    int64_t pre_fired = 0;
    for (int i = 0; i < fired_records.size(); i++)
    {
        const timerContext& ctx = fired_records[i];
        EXPECT_GE(ctx.fired, ctx.schedule + ctx.interval);
        if (ctx.interval > 0 && ctx.fired >(ctx.schedule + ctx.interval))
        {
            int value = (int)(ctx.fired - (ctx.schedule + ctx.interval));
            interval_tolerance.push_back(value);
        }
        if (pre_fired > 0)
        {
            EXPECT_GE(ctx.fired, pre_fired);
        }
        pre_fired = ctx.fired;
    }

    int sum = std::accumulate(interval_tolerance.begin(), interval_tolerance.end(), 0);
    printf("average tolerance: %f\n", (double)sum / (double)interval_tolerance.size());
}

TEST(TimerQueue, MinHeapTimerAdd)
{
    PQTimer timer;
    for (int i = 0; i < TRY; i++)
    {
        TestTimerAdd(&timer, N1);
    }
}

TEST(TimerQueue, TreeTimerAdd)
{
    TreeTimer timer;
    for (int i = 0; i < TRY; i++)
    {
        TestTimerAdd(&timer, N1);
    }
}

TEST(TimerQueue, WheelTimerAdd)
{
    WheelTimer timer;
    for (int i = 0; i < TRY; i++)
    {
        TestTimerAdd(&timer, N1);
    }
}

TEST(TimerQueue, MinHeapTimerExecute)
{
    PQTimer timer;
    TestTimerExpire(&timer, N2);
}

TEST(TimerQueue, TreeTimerExecute)
{
    TreeTimer timer;
    TestTimerExpire(&timer, N2);
}

TEST(TimerQueue, WheelTimerExecute)
{
    WheelTimer timer;
    TestTimerExpire(&timer, N2);
}