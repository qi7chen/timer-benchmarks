// Copyright © 2021 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#pragma once

#include "TimerBase.h"
#include <vector>
#include <unordered_map>


// timer scheduler implemented by priority queue(min-heap)
//
// complexity:
//     StartTimer  CancelTimer   PerTick
//      O(log N)    O(log N)       O(1)
//

struct TimerNode;

class PriorityQueueTimer : public TimerBase
{
public:


public:
    PriorityQueueTimer();
    ~PriorityQueueTimer();

    TimerSchedType Type() const override
    {
        return TimerSchedType::TIMER_PRIORITY_QUEUE;
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

private:
    std::vector<TimerNode*>  timers_; // binary timer heap
    std::unordered_map<int, TimerNode*> ref_; // to make O(1) lookup
};
