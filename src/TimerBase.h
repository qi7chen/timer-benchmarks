// Copyright © 2022 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#pragma once

#include <stdint.h>
#include <memory>
#include <functional>


enum class TimerSchedType
{
    TIMER_PRIORITY_QUEUE = 1,
    TIMER_QUAD_HEAP = 2,
    TIMER_RBTREE = 3,
    TIMER_HASHED_WHEEL = 4,
    TIMER_HH_WHEEL = 5,
};

// expiry action
typedef std::function<void()> TimeoutAction;

// we model 3 simple API for the construction and management of timers.
// 
//  1. int Start(interval, expiry_action)
//     start a timer that will expire after `interval` unit of time
//
//  2. void Stop(timer_id)
//    use `tiemr_id` to locate a timer and stop it
//
//  3. int Tick(now)
//    per-tick bookking routine
// 
class TimerBase
{
public:
    TimerBase();
    virtual ~TimerBase();

    TimerBase(const TimerBase&) = delete;
    TimerBase& operator=(const TimerBase&) = delete;

    virtual TimerSchedType Type() const = 0;

    // schedule a timer to run after specified time units(milliseconds).
    // returns an unique id identify this timer.
    // 
    // a `uint32_t` type of milliseconds means at most 49.7 days, that's good enough
    virtual int Start(uint32_t ms, TimeoutAction action) = 0;

    // cancel a timer by id
    // return true if successfully canceld
    virtual bool Cancel(int timer_id) = 0;

    // per-tick bookkeeping
    // return number of fired timers
    virtual int Tick(int64_t now) = 0;

    // count of pending timers.
    virtual int Size() const = 0;

protected:
    int nextId();

    int next_id_ = 2020;   // auto-increment timer id, with a magic  number
};

std::shared_ptr<TimerBase> CreateTimer(TimerSchedType sched_type);
