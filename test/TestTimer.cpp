// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include <chrono>
#include <thread>
#include <numeric>
#include <algorithm>
#include <memory>
#include <gtest/gtest.h>
#include "PQTimer.h"
#include "QuadTree.h"
#include "RBTreeTimer.h"
#include "WheelTimer.h"
#include "Clock.h"
#include "Benchmark.h"


using namespace std;

const int N1 = 1000;
const int N2 = 10;
const int TRY = 2;
const int TIME_DELTA = 10;

struct timerContext
{
    int id = 0;
    int interval = 0;           // interval of time units
    int64_t ts_schedule = 0;    // when timer scheduled to run
    int64_t ts_fired = 0;       // when timer fired
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
        timer->Schedule(0, callback);
    }
    
    // to make sure timing-wheel trigger all timers at next time unit
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_UNIT));

    EXPECT_EQ(timer->Size(), count);
    int fired = timer->Update();
    EXPECT_EQ(fired, count);
    EXPECT_EQ(called, count);
    EXPECT_EQ(timer->Size(), 0);
    called = 0;

    for (int i = 0; i < count; i++)
    {
        int id = timer->Schedule(0, callback);
        timer->Cancel(id);
    }
    fired = timer->Update();
    EXPECT_EQ(fired, 0);
    EXPECT_EQ(timer->Size(), 0);
    EXPECT_EQ(called, 0);

    doNotOptimizeAway(called);
    doNotOptimizeAway(fired);
}

static void TestTimerDel(TimerQueueBase* timer, int count) 
{

}

static void TestTimerExpire(TimerQueueBase* timer, int count)
{
    std::vector<timerContext> fired_records;
    std::vector<TimerCallback> timer_callbacks;
    fired_records.resize(count);
    timer_callbacks.resize(count);

    for (int i = 0; i < count; i++)
    {
        auto fn = [&](int idx)
        {
            fired_records[idx].ts_fired = Clock::CurrentTimeUnits(); // when timer fired
        };
        timer_callbacks[i] = std::bind(fn, i);
        int interval = (rand() % 100) + (i + 10) * TIME_DELTA;
        int id = timer->Schedule(interval, timer_callbacks[i]);

        fired_records[i].interval = interval;
        fired_records[i].ts_schedule = Clock::CurrentTimeUnits(); // when timer scheduled
        fired_records[i].id = id;
    }
    EXPECT_EQ(timer->Size(), count);

    // execute all timers
    printf("start execute timer at %lld\n", Clock::CurrentTimeUnits());
    int fired = 0;
    while (fired < count)
    {
        fired += timer->Update();
    }

    EXPECT_EQ(timer->Size(), 0);
    
    std::vector<int> interval_tolerance;
    interval_tolerance.reserve(count);

    for (int i = 0; i < fired_records.size(); i++)
    {
        const timerContext& ctx = fired_records[i];
        EXPECT_GE(ctx.ts_fired, ctx.ts_schedule + ctx.interval);
        if (ctx.ts_fired < ctx.ts_schedule + ctx.interval)
        {
            printf("timer %d failed\n", ctx.id);
        }
        if (ctx.interval > 0 && ctx.ts_fired > (ctx.ts_schedule + ctx.interval))
        {
            int value = (int)(ctx.ts_fired - (ctx.ts_schedule + ctx.interval));
            interval_tolerance.push_back(value);
        }
    }

    int sum = std::accumulate(interval_tolerance.begin(), interval_tolerance.end(), 0);
    printf("average tolerance: %f\n", (double)sum / (double)interval_tolerance.size());
}

std::vector<TimerQueueBase*>  createTimers() 
{
    std::vector<TimerQueueBase*> timers;
    timers.push_back(new PQTimer);
    timers.push_back(new QuadTreeTimer);
    timers.push_back(new RBTreeTimer);
    timers.push_back(new WheelTimer);
    return timers;
}

void clearTimers(std::vector<TimerQueueBase*>& timers)
{
    for (int i = 0; i < timers.size(); i++)
    {
        delete timers[i];
    }
    timers.clear();
}

TEST(TimerQueue, TimerAdd)
{
    auto timers = createTimers();
    for (int i = 0; i < timers.size(); i++)
    {
        auto timer = timers[i];
        TestTimerAdd(timer, N1);
    }
    clearTimers(timers);
}

TEST(TimerQueue, TimerDel)
{
    auto timers = createTimers();
    for (int i = 0; i < timers.size(); i++)
    {
        auto timer = timers[i];
        TestTimerDel(timer, N1);
    }
    clearTimers(timers);
}


TEST(TimerQueue, TimerExecute)
{
    auto timers = createTimers();
    for (int i = 0; i < timers.size(); i++)
    {
        auto timer = timers[i];
        TestTimerExpire(timer, N1);
    }
    clearTimers(timers);
}

