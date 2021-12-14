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
    last_time_ = Clock::CurrentTimeMillis();
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


int HashedWheelTimer::Start(uint32_t ms, TimeoutAction action)
{
    int id = nextId();
    int64_t deadline = Clock::CurrentTimeMillis() + ms;
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
    if (size_ == 0) 
    {
        return 0;
    }
    if (now == 0) {
        now = Clock::CurrentTimeMillis();
    }
    int64_t elapsed = now - last_time_;
    int64_t ticks = elapsed / TIME_UNIT;
    if (ticks <= 0)
    {
        return 0;
    }
    last_time_ = now;
    int fired = 0;
    for (int64_t i = 0; i < ticks; i++)
    {
        fired += tick(now);
    }
    return fired;
}

int HashedWheelTimer::tick(int64_t now)
{
    int64_t deadline = started_at_ + TICK_DURATION * (ticks_ + 1);
    if (now < deadline) {
        return 0;
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