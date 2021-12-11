// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"

// Hashed and Hierarchical Timing Wheels
// see (http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf)

class HHWheelTimer : public TimerBase
{
public:


public:
    HHWheelTimer();
    ~HHWheelTimer();

    int Start(uint32_t time_units, TimeoutAction action) override;

    bool Stop(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override
    {
        return 0; // TODO:
    }

private:


};
