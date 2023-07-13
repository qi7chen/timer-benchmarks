// Copyright © 2022 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#pragma once

#include "TimerBase.h"
#include <vector>
#include <unordered_map>

// Quaternary-ary heap
// https://en.wikipedia.org/wiki/D-ary_heap
// 
// timer scheduler implemented by quaternary-ary heap
//
// complexity:
//     StartTimer    CancelTimer   PerTick
//      O(logN)      O(logN)          O(1)
//

struct TimerNode;

class QuadHeapTimer : public TimerBase
{public:
    QuadHeapTimer();
    ~QuadHeapTimer();

    TimerSchedType Type() const override
    {
        return TimerSchedType::TIMER_QUAD_HEAP;
    }

    // start a timer after `duration` milliseconds
    int Start(uint32_t duration, TimeoutAction action) override;

    // cancel a timer
    bool Cancel(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override 
    {
        return (int)timers_.size();
    }    

private:
    void clear();
    int delTimer(TimerNode& node);

    std::vector<TimerNode*>  timers_; // 4-ary heap
    std::unordered_map<int, TimerNode*> ref_; // O(1) search
};
