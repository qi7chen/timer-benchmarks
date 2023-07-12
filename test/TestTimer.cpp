// Copyright © 2021 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#include <chrono>
#include <thread>
#include <numeric>
#include <vector>
#include <algorithm>
#include <unordered_map>
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

struct TimeOutContext {
    int id = 0;
    int interval = 0;
    int64_t deadline = 0;
    int64_t fired_at = 0;
};

static void TestTimerAdd(TimerBase *timer, int count) {
    int called = 0;
    for (int i = 0; i < count; i++) {
        timer->Start(0, [&]() {
            called++;
        });
    }

    // to make sure timing-wheel trigger all timers at next time unit
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    EXPECT_EQ(timer->Size(), count);
    int fired = timer->Tick(Clock::CurrentTimeMillis());
    EXPECT_EQ(fired, count);
    EXPECT_EQ(called, count);
    EXPECT_EQ(timer->Size(), 0);

    called = 0;
    for (int i = 0; i < count; i++) {
        int id = timer->Start(0, [&]() {
            called++;
        });
        timer->Cancel(id);
    }
    fired = timer->Tick(Clock::CurrentTimeMillis());
    EXPECT_EQ(fired, 0);
    EXPECT_EQ(timer->Size(), 0);
    EXPECT_EQ(called, 0);

    doNotOptimizeAway(called);
    doNotOptimizeAway(fired);
}

static void TestTimerDel(TimerBase *timer, int count) {
    int called = 0;
    int tid = timer->Start(100, [&]() {
        called++;
    });

    timer->Tick(Clock::CurrentTimeMillis());
    timer->Cancel(tid);

    EXPECT_EQ(called, 0);
}

static void TestTimerExpire(TimerBase *timer, int count) {
    int64_t max_interval = 0;
    std::unordered_map<int, TimeOutContext*> timedOut;
    for (int i = 0; i < count; i++) {
        int interval = TIME_DELTA + (rand() % 100);
        TimeOutContext* ctx = new(TimeOutContext);
        ctx->interval = interval;
        ctx->deadline = Clock::CurrentTimeMillis() + interval;
        if (max_interval < interval) {
            max_interval = interval;
        }
        int id = timer->Start(interval, [=]() {
            //printf("timer %d fired\n", ctx->id);
            ctx->fired_at = Clock::CurrentTimeMillis();
        });
        ctx->id = id;
    }
    EXPECT_EQ(timer->Size(), count);

    // execute all timers
    auto now = Clock::CurrentTimeString(Clock::CurrentTimeMillis());
    printf("start execute timer at %s\n", now.c_str());

    int fired = 0;
    for (int i = 0; i <= max_interval; i++) {
        fired += timer->Tick(Clock::CurrentTimeMillis());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_EQ(timer->Size(), 0);

    for (const auto& kv : timedOut) {
        TimeOutContext* ctx = kv.second;
        EXPECT_GE(ctx->fired_at, ctx->deadline);
        int64_t duration = ctx->fired_at > ctx->deadline;
        if (duration < 0) {
            printf("timer %d failed %lld\n", ctx->id, duration);
        }
    }
}

// same deadline timers should expired in FIFO order
static void TestTimerExpireFIFO(TimerBase *timer) {
    std::vector<TimeOutContext*> expired;
    int64_t deadline = Clock::CurrentTimeMillis() + 100;
    for (int i = 0; i < 50; i++) {
        uint32_t duration = uint32_t(deadline - Clock::CurrentTimeMillis());
        TimeOutContext* ctx = new(TimeOutContext);
        ctx->deadline = deadline;
        ctx->interval = duration;
        int tid = timer->Start(duration, [=,&expired]() {
            //printf("timer %d fired\n", ctx->id);
            ctx->fired_at = Clock::CurrentTimeMillis();
            expired.push_back(ctx);
        });
        ctx->id = tid;
    }
    for (int i = 0; i < 100; i++) {
        timer->Tick(Clock::CurrentTimeMillis());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    timer->Tick(Clock::CurrentTimeMillis());

    EXPECT_EQ(expired.size(), 50);

    cout << "expire order: ";
    for (int i = 0; i < expired.size(); i++)
    {
        cout << expired[i]->id << " ";
    }
    cout << endl;
    
    bool sorted = std::is_sorted(expired.begin(), expired.end(), [](TimeOutContext* a, TimeOutContext* b) {
        return a->id < b->id;
    });
    bool reverseSorted = std::is_sorted(expired.begin(), expired.end(), [](TimeOutContext* a, TimeOutContext* b) {
        return a->id > b->id;
    });

    if (sorted) {
        printf("timer type %d is expired in FIFO order\n", timer->Type());
    } else if (reverseSorted) {
        printf("timer type %d is expired in FILO order\n", timer->Type());
    } else {
        printf("timer type %d is expired out of order\n", timer->Type());
    }
}


TEST(TimerPriorityQueue, TimerAdd) {
    auto timer = CreateTimer(TimerSchedType::TIMER_PRIORITY_QUEUE);
    TestTimerAdd(timer.get(), N1);
}

TEST(TimerPriorityQueue, TimerDel) {
    auto timer = CreateTimer(TimerSchedType::TIMER_PRIORITY_QUEUE);
    TestTimerDel(timer.get(), N1);
}


TEST(TimerPriorityQueue, TimerExpireDelay) {
    auto timer = CreateTimer(TimerSchedType::TIMER_PRIORITY_QUEUE);
    TestTimerExpire(timer.get(), N1);
}

TEST(TimerPriorityQueue, TimerExpireFIFO) {
    auto timer = CreateTimer(TimerSchedType::TIMER_PRIORITY_QUEUE);
    TestTimerExpireFIFO(timer.get());
}

///////////////////////////////////////////////////////////////////


TEST(TimerQuadHeap, TimerAdd) {
    auto timer = CreateTimer(TimerSchedType::TIMER_QUAD_HEAP);
    TestTimerAdd(timer.get(), N1);
}

TEST(TimerQuadHeap, TimerDel) {
    auto timer = CreateTimer(TimerSchedType::TIMER_QUAD_HEAP);
    TestTimerDel(timer.get(), N1);
}


TEST(TimerQuadHeap, TimerExpireDelay) {
    auto timer = CreateTimer(TimerSchedType::TIMER_QUAD_HEAP);
    TestTimerExpire(timer.get(), N1);
}

TEST(TimerQuadHeap, TimerExpireFIFO) {
    auto timer = CreateTimer(TimerSchedType::TIMER_QUAD_HEAP);
    TestTimerExpireFIFO(timer.get());
}


/////////////////////////////////////////////////////////////////

TEST(TimerRBTree, TimerAdd) {
    auto timer = CreateTimer(TimerSchedType::TIMER_RBTREE);
    TestTimerAdd(timer.get(), N1);
}

TEST(TimerRBTree, TimerDel) {
    auto timer = CreateTimer(TimerSchedType::TIMER_RBTREE);
    TestTimerDel(timer.get(), N1);
}


TEST(TimerRBTree, TimerExecute) {
    auto timer = CreateTimer(TimerSchedType::TIMER_RBTREE);
    TestTimerExpire(timer.get(), N1);
}

TEST(TimerRBTree, TimerExpireFIFO) {
    auto timer = CreateTimer(TimerSchedType::TIMER_RBTREE);
    TestTimerExpireFIFO(timer.get());
}


/////////////////////////////////////////////////////////////////

TEST(TimerHashedWheel, TimerAdd) {
    auto timer = CreateTimer(TimerSchedType::TIMER_HASHED_WHEEL);
    TestTimerAdd(timer.get(), N1);
}

TEST(TimerHashedWheel, TimerDel) {
    auto timer = CreateTimer(TimerSchedType::TIMER_HASHED_WHEEL);
    TestTimerDel(timer.get(), N1);
}


TEST(TimerHashedWheel, TimerExecute) {
    auto timer = CreateTimer(TimerSchedType::TIMER_HASHED_WHEEL);
    TestTimerExpire(timer.get(), N1);
}

TEST(TimerHashedWheel, TimerExpireFIFO) {
    auto timer = CreateTimer(TimerSchedType::TIMER_HASHED_WHEEL);
    TestTimerExpireFIFO(timer.get());
}


///////////////////////////////////////////////////////////////////////

TEST(TimerHHWheel, TimerAdd) {
    auto timer = CreateTimer(TimerSchedType::TIMER_HH_WHEEL);
    TestTimerAdd(timer.get(), N1);
}

TEST(TimerHHWheel, TimerDel) {
    auto timer = CreateTimer(TimerSchedType::TIMER_HH_WHEEL);
    TestTimerDel(timer.get(), N1);
}


TEST(TimerHHWheel, TimerExecute) {
    auto timer = CreateTimer(TimerSchedType::TIMER_HH_WHEEL);
    TestTimerExpire(timer.get(), N1);
}

TEST(TimerHHWheel, TimerExpireFIFO) {
    auto timer = CreateTimer(TimerSchedType::TIMER_HH_WHEEL);
    TestTimerExpireFIFO(timer.get());
}
