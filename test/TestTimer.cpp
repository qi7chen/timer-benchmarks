// Copyright (C) 2018 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include <chrono>
#include <thread>
#include <numeric>
#include <algorithm>
#include <memory>
#include <gtest/gtest.h>
#include "Clock.h"
#include "Benchmark.h"
#include "TimerBase.h"


using namespace std;

const int N1 = 1000;
const int N2 = 10;
const int TRY = 2;
const int TIME_DELTA = 10;

struct timerContext
{
    int id = 0;
    int interval = 0;           // interval of time units
    int64_t started_at = 0;     // when timer scheduled to run
    int64_t fired_at = 0;       // when timer fired
};

static void TestTimerAdd(TimerBase* timer, int count)
{
    int called = 0;
    auto callback = [&]()
    {
        called++;
    };
    for (int i = 0; i < count; i++)
    {
        timer->Start(0, callback);
    }
    
    // to make sure timing-wheel trigger all timers at next time unit
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_UNIT));

    EXPECT_EQ(timer->Size(), count);
    int fired = timer->Tick();
    EXPECT_EQ(fired, count);
    EXPECT_EQ(called, count);
    EXPECT_EQ(timer->Size(), 0);
    called = 0;

    for (int i = 0; i < count; i++)
    {
        int id = timer->Start(0, callback);
        timer->Stop(id);
    }
    fired = timer->Tick();
    EXPECT_EQ(fired, 0);
    EXPECT_EQ(timer->Size(), 0);
    EXPECT_EQ(called, 0);

    doNotOptimizeAway(called);
    doNotOptimizeAway(fired);
}

static void TestTimerDel(TimerBase* timer, int count)
{
    int called = 0;
    int tid = timer->Start(1000, [&]() {
        called++;
    });

    timer->Tick();
    timer->Stop(tid);

    EXPECT_EQ(called, 0);
}

static void TestTimerExpire(TimerBase* timer, int count)
{
    std::vector<timerContext> fired_records;
    std::vector<TimeoutAction> timer_actions;
    fired_records.resize(count);
    timer_actions.resize(count);

    for (int i = 0; i < count; i++)
    {
        auto fn = [&](int idx)
        {
            fired_records[idx].fired_at = Clock::CurrentTimeMillis(); // when timer fired
        };
        timer_actions[i] = std::bind(fn, i);
        int interval = (rand() % 100) + (i + 10) * TIME_DELTA;
        int id = timer->Start(interval, timer_actions[i]);

        fired_records[i].interval = interval;
        fired_records[i].started_at = Clock::CurrentTimeMillis(); // when timer scheduled
        fired_records[i].id = id;
    }
    EXPECT_EQ(timer->Size(), count);

    // execute all timers
    printf("start execute timer at %lld\n", Clock::CurrentTimeMillis());

    int fired = 0;
    for (int i = 0; fired < count; i++)
    {
        fired += timer->Tick();
        if (i > 0 && i % 100 == 0) {
            Clock::TimeFly(TIME_UNIT); // time faster
        }
    }

    EXPECT_EQ(timer->Size(), 0);
    
    std::vector<int> interval_tolerance;
    interval_tolerance.reserve(count);

    for (int i = 0; i < fired_records.size(); i++)
    {
        const timerContext& ctx = fired_records[i];
        EXPECT_GE(ctx.fired_at, ctx.started_at + ctx.interval);
        if (ctx.fired_at < ctx.started_at + ctx.interval)
        {
            printf("timer %d failed\n", ctx.id);
        }
        if (ctx.interval > 0 && ctx.fired_at > (ctx.started_at + ctx.interval))
        {
            int value = (int)(ctx.fired_at - (ctx.started_at + ctx.interval));
            interval_tolerance.push_back(value);
        }
    }

    Clock::TimeReset();

    int sum = std::accumulate(interval_tolerance.begin(), interval_tolerance.end(), 0);
    printf("average tolerance: %fms\n", (double)sum / (double)interval_tolerance.size());
}

std::vector<std::shared_ptr<TimerBase>>  createTimers()
{
    std::vector<std::shared_ptr<TimerBase>> timers;
    //timers.push_back(CreateTimer(TimerSchedType::TIMER_PRIORITY_QUEUE));
    timers.push_back(CreateTimer(TimerSchedType::TIMER_QUAD_HEAP));
    //timers.push_back(CreateTimer(TimerSchedType::TIMER_RBTREE));
    //timers.push_back(CreateTimer(TimerSchedType::TIMER_HASHED_WHEEL));
    //timers.push_back(CreateTimer(TimerSchedType::TIMER_HH_WHEEL));
    return std::move(timers);
}



TEST(TimerQueue, TimerAdd)
{
    auto timers = createTimers();
    for (int i = 0; i < timers.size(); i++)
    {
        auto timer = timers[i];
        TestTimerAdd(timer.get(), N1);
    }
}

TEST(TimerQueue, TimerDel)
{
    auto timers = createTimers();
    for (int i = 0; i < timers.size(); i++)
    {
        auto timer = timers[i];
        TestTimerDel(timer.get(), N1);
    }
}


TEST(TimerQueue, TimerExecute)
{
    auto timers = createTimers();
    for (int i = 0; i < timers.size(); i++)
    {
        auto timer = timers[i];
        TestTimerExpire(timer.get(), N1);
    }
}

