// Copyright (C) 2018 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include <stdint.h>
#include <memory>
#include "define.h"


// basic operations of a timer 
class TimerBase
{
public:
    TimerBase();
    virtual ~TimerBase();

    TimerBase(const TimerBase&) = delete;
    TimerBase& operator=(const TimerBase&) = delete;

    // schedule a timer to run after specified time units(milliseconds).
    // returns an unique id identify this timer.
    virtual int Start(uint32_t ms, TimeoutAction action) = 0;

    // cancel a timer by id.
    virtual bool Stop(int timer_id) = 0;

    // per-tick bookkeeping.
    virtual int Tick(int64_t now = 0) = 0;

    // count of pengding timers.
    virtual int Size() const = 0;

protected:
    int nextId();

    int next_id_ = 0;   // autoincrement timer id 
};

std::shared_ptr<TimerBase> CreateTimer(TimerSchedType sched_type);

