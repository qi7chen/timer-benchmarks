// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HHWheelTimer.h"
#include "Clock.h"

HHWheelTimer::HHWheelTimer()
{
    last_time_ = Clock::CurrentTimeMillis();
}


HHWheelTimer::~HHWheelTimer()
{
    ref_.clear();
}


int HHWheelTimer::Start(uint32_t ms, TimeoutAction action)
{
    int id = nextId();
    WheelTimerNode* node = new WheelTimerNode();
    node->id = id;
    node->action = action;
    node->deadline = Clock::CurrentTimeMillis() + ms;
    wheel_.AddNode(node);
    ref_[id] = node;
    return 0;
}

bool HHWheelTimer::Stop(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter == ref_.end()) {
        return false;
    }
    
    deltimer(iter->second);
    return true;
}

void HHWheelTimer::deltimer(WheelTimerNode* node)
{
    node->Remove();
    ref_.erase(node->id);
    delete node;
}


int HHWheelTimer::Tick(int64_t now)
{
    if (Size() == 0)
    {
        return 0;
    }
    if (now == 0) 
    {
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
        fired += wheel_.Tick();
    }
    return fired;
}
