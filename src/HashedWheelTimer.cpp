// Copyright (C) 2021 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HashedWheelTimer.h"
#include "HashedWheelBucket.h"
#include <thread>
#include <chrono>
#include "Clock.h"
#include "Logging.h"

const int WHEEL_SIZE = 512;
const int64_t TICK_DURATION = 100;  // milliseconds
const int64_t TIME_UNIT = 10;       // 10ms

HashedWheelTimer::HashedWheelTimer()
{
    started_at_ = Clock::CurrentTimeMillis();
    last_time_ = Clock::CurrentTimeMillis();
    wheel_.resize(WHEEL_SIZE);
    for (int i = 0; i < WHEEL_SIZE; i++) {
        wheel_[i] = new HashedWheelBucket();
    }
}


HashedWheelTimer::~HashedWheelTimer()
{
    purge();
}

void HashedWheelTimer::purge()
{
    for (int i = 0; i < (int)wheel_.size(); i++) {
        HashedWheelBucket* bucket = wheel_[i];
        HashedWheelTimeout* node = bucket->head;
        while (node != nullptr) {
            HashedWheelTimeout* next = node->next;
            delTimeout(node);
            node = next;
        }
        bucket->head = bucket->tail = nullptr;
        delete wheel_[i];
    }
    wheel_.clear();
}


int HashedWheelTimer::Start(uint32_t ms, TimeoutAction action)
{
    int id = nextId();
    int64_t deadline = Clock::CurrentTimeMillis() + ms;
    HashedWheelTimeout* timeout = allocTimeout(id, deadline, action);
    int calculated = (int)(timeout->deadline - started_at_) / TICK_DURATION;
    timeout->remaining_rounds = (calculated - ticks_) / WHEEL_SIZE;
    int ticks = calculated < ticks_ ? ticks_ : calculated;
    int stop_idx = ticks & (WHEEL_SIZE - 1);
    wheel_[stop_idx]->AddTimeout(timeout);
    ref_[id] = timeout;
    return id;
}

bool HashedWheelTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter == ref_.end()) {
        return false;
    }
    HashedWheelTimeout* timeout = iter->second;
    if (timeout != nullptr) {
        timeout->bucket->Remove(timeout);
        delTimeout(timeout);
    }
    return true;
}

int HashedWheelTimer::Tick(int64_t now)
{
    if (Size() == 0) 
    {
        return 0;
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
        fired += tick();
    }
    return fired;
}

int HashedWheelTimer::tick()
{
    int64_t deadline = started_at_ + TICK_DURATION * (ticks_ + 1);
    int idx = ticks_ % (WHEEL_SIZE - 1);
    HashedWheelBucket* bucket = wheel_[idx];
    std::vector<HashedWheelTimeout*> expired;
    bucket->ExpireTimeouts(deadline, expired);
    int count = (int)expired.size();
    for (int i = 0; i < (int)expired.size(); i++) {
        HashedWheelTimeout* timeout = expired[i];
        timeout->Expire();
        delTimeout(timeout);
    }
    ticks_++;
    return count;
}


void HashedWheelTimer::delTimeout(HashedWheelTimeout* timeout)
{
    ref_.erase(timeout->id);
    freeTimeout(timeout);
}

HashedWheelTimeout* HashedWheelTimer::allocTimeout(int id, int64_t deadline, TimeoutAction action)
{
    return new HashedWheelTimeout(id, deadline, action);
}

void HashedWheelTimer::freeTimeout(HashedWheelTimeout* p)
{
    delete p;
}
