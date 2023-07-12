// Copyright (C) 2021 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HHWheelTimer.h"
#include "Clock.h"
#include "Logging.h"
#include <assert.h>


struct TimerNode {
    timer_list entry;
    int id = 0;
    TimeoutAction action;
};


inline int64_t current_clock() {
    return Clock::CurrentTimeMillis();
}


HHWheelTimer::HHWheelTimer()
{
    int64_t now = Clock::CurrentTimeMillis();
    started_at_ = now;
    init_timers(&base_, now);
    ref_.rehash(1024);
}


HHWheelTimer::~HHWheelTimer()
{
    clear();
}

void HHWheelTimer::clear()
{
    init_timers(&base_, 0);
    for (const auto& kv : ref_) {
        delete kv.second;
    }
    ref_.clear();
}


int HHWheelTimer::Start(uint32_t duration, TimeoutAction action)
{
    int id = nextId();
    TimerNode* node = new TimerNode();
    node->id = id;
    node->entry.expires = Clock::CurrentTimeMillis() + duration;
    node->entry.function = HHWheelTimer::timerExpireCallback;
    node->entry.base = &base_;

    add_timer(&node->entry);
    ref_[id] = node;
    return id;
}

bool HHWheelTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter != ref_.end()) {
        TimerNode* node = iter->second;
        del_timer(&node->entry);
        delete node;
        return true;
    }
    return false;
}

int HHWheelTimer::Tick(int64_t ticks)
{
    return run_timers(&base_, ticks);
}

void HHWheelTimer::timerExpireCallback(timer_list* timer)
{
    assert(timer);
    TimerNode* node = container_of(timer, TimerNode, entry);
    HHWheelTimer* wheel = container_of(timer->base, HHWheelTimer, base_);
    node->action();
    wheel->Cancel(node->id);
}

