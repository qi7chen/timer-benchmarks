// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"
#include "HHWheel.h"
#include <unordered_map>

// Hashed and Hierarchical Timing Wheels
// see (http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf)

class HHWheelTimer : public TimerBase
{
public:


public:
    HHWheelTimer();
    ~HHWheelTimer();

    // start a timer after `ms` milliseconds
    int Start(uint32_t ms, TimeoutAction action) override;

    // stop a timer
    bool Stop(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override
    {
        return (int)ref_.size();
    }

private:
    void deltimer(WheelTimerNode*);

private:
    int64_t last_time_ = 0;
    HHTimingWheel wheel_;
    std::unordered_map<int, WheelTimerNode*> ref_;
};
