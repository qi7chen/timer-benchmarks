// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HashedWheelTimer.h"
#include "HashedWheelBucket.h"
#include <thread>
#include <chrono>
#include "Clock.h"
#include "Logging.h"

const int WHEEL_SIZE = 512;
const int TICK_DURATION = 100; // milliseconds

HashedWheelTimer::HashedWheelTimer()
{
    wheel_.resize(WHEEL_SIZE);
    for (int i = 0; i < WHEEL_SIZE; i++) {
        wheel_[i] = new HashedWheelBucket();
    }
    started_at_ = Clock::CurrentTimeMillis();
}


HashedWheelTimer::~HashedWheelTimer()
{
    purge();
}

void HashedWheelTimer::purge()
{
    for (int i = 0; i < (int)wheel_.size(); i++) {
        delete wheel_[i];
    }
    wheel_.clear();
}


int HashedWheelTimer::Start(uint32_t time_units, TimeoutAction action)
{
    int id = nextId();
    int64_t deadline = Clock::CurrentTimeMillis() + time_units;
    HashedWheelTimeout* timeout = new HashedWheelTimeout(this, id, deadline, action);
    timeouts_.push(timeout);
    ref_[id] = timeout;
    return 0;
}

bool HashedWheelTimer::Stop(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter == ref_.end()) {
        return false;
    }
    HashedWheelTimeout* timeout = iter->second;
    if (timeout != nullptr) {
        timeout->Cancel();
    }
    return true;
}

int HashedWheelTimer::Tick(int64_t now)
{
    int64_t deadline = started_at_ + TICK_DURATION * (ticks_ + 1);
    if (now < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(deadline - now));
    }
    int idx = ticks_ % (WHEEL_SIZE - 1);
    processCancelledTasks();
    HashedWheelBucket* bucket = wheel_[idx];
    transferTimeoutToBuckets();

    std::vector<HashedWheelTimeout*> expired;
    bucket->ExpireTimeouts(deadline, expired);
    int count = (int)expired.size();
    for (int i = 0; i < (int)expired.size(); i++) {
        HashedWheelTimeout* timeout = expired[i];
        timeout->Expire();
        delete expired[i];
    }
    ticks_++;
    return count;
}


void HashedWheelTimer::processCancelledTasks()
{
    while(!cancelled_timeouts_.empty())
    {
        HashedWheelTimeout* timeout = cancelled_timeouts_.front();
        cancelled_timeouts_.pop();
        if (timeout != nullptr) {
            timeout->Remove();
        }
    }
}

void HashedWheelTimer::transferTimeoutToBuckets()
{
    // transfer only max. 100000 timeouts per tick to prevent a thread
    // to stale when it just  adds new timeouts in a loop.
    for (int i = 0; i < 100000 && !timeouts_.empty(); i++)
    {
        HashedWheelTimeout* timeout = timeouts_.front();
        timeouts_.pop();
        if (timeout == nullptr) {
            continue;
        }
        if (timeout->IsCanceled()) {
            continue;
        }
        int calculated = (int)(timeout->deadline - started_at_) / TICK_DURATION;
        timeout->remaining_rounds = (calculated - ticks_) / WHEEL_SIZE;
        int ticks = calculated < ticks_ ? ticks_ : calculated;
        int stop_idx = ticks & (WHEEL_SIZE - 1);
        wheel_[stop_idx]->AddTimeout(timeout);
    }
}