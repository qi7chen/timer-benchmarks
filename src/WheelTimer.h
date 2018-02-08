// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "ITimer.h"

// timer queue implemented with hashed hierarchical wheel.
//
// We model timers as the number of ticks until the next
// due event.We allow 32 - bits of space to track this
// due interval, and break that into 4 regions of 8 bits.
// Each region indexes into a bucket of 256 lists.
//
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(1)       O(1)          O(1)
//
class WheelTimer : public ITimerQueue
{
public:
    WheelTimer();
    ~WheelTimer();

    int AddTimer(int millsec, TimerCallback cb) override;

    void CancelTimer(int id) override;

    void Tick(int64_t now) override;

private:

};

