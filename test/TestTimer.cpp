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

const int MaxCount = 20;
const int SleepInterval = 5;

template <typename T>
class TimerQueueTest
{
public:
    TimerQueueTest()
    {
        interval = 20;
        timer.AddTimer(interval, std::bind(&TimerQueueTest::onTimeout, this, 1));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        timer.AddTimer(interval, std::bind(&TimerQueueTest::onTimeout, this, 2));
    }

    void onTimeout(int i)
    {
        int64_t now = GetNowTime();

        count++;
        if (count < MaxCount)
        {
            timer.AddTimer(interval, std::bind(&TimerQueueTest::onTimeout, this, i)); // repeat again
        }

        const std::string& timestamp = CurrentTimeString(now);
        printf("%s timer %d expired %d\n", timestamp.c_str(), i, count);

        if (lastExpire > 0)
        {
            int64_t elapsed = now - lastExpire;
            assert(elapsed >= interval);
            assert(elapsed - interval <= 2*SleepInterval);
        }
        lastExpire = now;
    }

    void Run()
    {
        while (count < MaxCount)
        {
            auto now = std::chrono::system_clock::now();
            auto timepoint = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            timer.Tick(timepoint);
            std::this_thread::sleep_for(std::chrono::milliseconds(SleepInterval));
        }
    }

private:
    T timer;
    int interval = 0;
    int64_t lastExpire = 0;
    int count = 0;
};

TEST(TimerQueue, TestAddTimer)
{
    TimerQueueTest<PQTimer> test1;
    test1.Run();

    //TimerQueueTest<TreeTimer> test2;
    //test2.Run();
}