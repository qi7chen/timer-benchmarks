// Copyright © 2022 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#pragma once

#include "TimerBase.h"
#include "timer_list.h"
#include <unordered_map>

// Hashed and Hierarchical Timing Wheels
// see https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/kernel/timer.c?h=linux-3.10.y
//
// timer scheduler implemented by hashed & hierachical wheels
// complexity:
//      StartTimer   CancelTimer   PerTick
//       O(1)         O(1)          O(1)
//

struct TimerNode;

class HHWheelTimer : public TimerBase
{
public:
    HHWheelTimer();
    ~HHWheelTimer();

    // start a timer after `duration` milliseconds
    int Start(uint32_t duration, TimeoutAction action) override;

    // cancel a timer
    bool Cancel(int timer_id) override;

    // we assume 1 tick per ms
    int Tick(int64_t ticks) override;

    int Size() const override
    {
        return (int)ref_.size();
    }

private:
    void clear();
    static void timerExpireCallback(timer_list* timer);

private:
    int64_t started_at_ = 0;
    tvec_base base_;
    std::unordered_map<int, TimerNode*> ref_;
};
