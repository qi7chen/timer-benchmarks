// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include "PQTimer.h"
#include "TreeTimer.h"
#include "WheelTimer.h"
#include "Clock.h"

enum
{
    ScheduleRange = 100,
    MaxTimeoutCount = 20,
};


struct TimerContext
{
    int interval = 0;
    int id = 0;
    int timeoutCount = 0;
    int64_t lastExpire = 0;
    TimerQueueBase* queue = nullptr;
};

static void onTimeout(TimerContext* ctx)
{
    int64_t now = Clock::CurrentTimeMillis();
    ctx->timeoutCount++;

    if (ctx->lastExpire > 0)
    {
        int64_t elapsed = now - ctx->lastExpire;
        int64_t diff = elapsed - ctx->interval;
        EXPECT_GE(elapsed, ctx->interval);
    }
    ctx->lastExpire = now;

    const std::string& timestamp = Clock::CurrentTimeString(now);
    printf("%s timer[%d] expired of %d\n", timestamp.c_str(), ctx->id, ctx->timeoutCount);

    if (ctx->timeoutCount < MaxTimeoutCount)
    {
        int interval = rand() % 50;
        ctx->interval = interval;
        ctx->queue->AddTimer(interval, std::bind(&onTimeout, ctx)); // repeat again
    }
    if (ctx->id % 2 == 0 && ctx->timeoutCount == MaxTimeoutCount / 2)
    {
        printf("cancel timer %d of %d\n", ctx->id, ctx->timeoutCount);
        ctx->queue->CancelTimer(ctx->id);
    }
}

static void TestTimerQueue(TimerQueueBase* timer, int count)
{
    std::vector<TimerContext> ctxvec;
    ctxvec.resize(count);
    for (int i = 0; i < count; i++)
    {
        TimerContext* ctx = &ctxvec[i];
        ctx->queue = timer;
        ctx->interval = rand() % ScheduleRange;
        int id = timer->AddTimer(ctx->interval, std::bind(&onTimeout, ctx));
        ctx->id = id;
        //printf("schedule timer %d of interval %d\n", id, ctx->interval);
        //std::this_thread::sleep_for(std::chrono::milliseconds(ctx->interval/2));
    }
    for (int i = 0; i < count; i++)
    {
        if (count % 2 == 0)
        {
            timer->CancelTimer(i);
        }
    }
    printf("all timers scheduled, %d\n", count / 2);
    while (timer->Size() > 0)
    {
        timer->Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

TEST(TimerQueue, TestPQTimer)
{
    PQTimer timer;
    TestTimerQueue(&timer, 100);
}

TEST(TimerQueue, TestTreeTimer)
{
    TreeTimer timer;
    TestTimerQueue(&timer, 100);
}

TEST(TimerQueue, TestWheelTimer)
{
    WheelTimer timer;
    TestTimerQueue(&timer, 100);
}
