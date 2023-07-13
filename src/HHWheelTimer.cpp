// Copyright © 2021 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE.txt

#include "HHWheelTimer.h"
#include "Clock.h"
#include "Logging.h"
#include <assert.h>

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
    timer_list* timer = new timer_list();
    timer->id = id;
    timer->base = &base_;
    timer->data = this;
    timer->expires = Clock::CurrentTimeMillis() + (int64_t)duration;
    timer->function = HHWheelTimer::handleTimerExpired;

    add_timer(timer);
    ref_[id] = timer;
    actions_[id] = action;
    return id;
}

bool HHWheelTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter == ref_.end()) {
        return false;
    }

    timer_list* timer = iter->second;
    del_timer(timer);
    ref_.erase(timer_id);
    actions_.erase(timer_id);

    timer->base = NULL;
    timer->function = NULL;
    delete timer;

    return true;
}

int HHWheelTimer::Tick(int64_t ticks)
{
    return run_timers(&base_, ticks);
}

TimeoutAction HHWheelTimer::findAndDelAction(int id)
{
    auto iter = actions_.find(id);
    if (iter != actions_.end())
    {
        TimeoutAction action = std::move(iter->second);
        actions_.erase(iter);
        return std::move(action);
    }
    return nullptr;
}

void HHWheelTimer::handleTimerExpired(timer_list* timer)
{
    assert(timer);
    auto wheel = reinterpret_cast<HHWheelTimer*>(timer->data);
    TimeoutAction action = wheel->findAndDelAction(timer->id);
    wheel->Cancel(timer->id);

    if (action) {
        action();
    }
}

