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
class QuatHeapTimer : public TimerBase
{
public:
    struct TimerNode
    {
        int index = -1;    // array index
        int id = -1;       // unique timer id
        int64_t deadline = 0;  
        TimeoutAction action = nullptr;

        bool operator < (const TimerNode& b) const
        {
            if (deadline == b.deadline) {
                return id > b.id;
            }
            return deadline < b.deadline;
        }
    };

public:
    QuatHeapTimer();
    ~QuatHeapTimer();

    TimerSchedType Type() const override
    {
        return TimerSchedType::TIMER_QUAD_HEAP;
    }

    // start a timer after `ms` milliseconds
    int Start(uint32_t ms, TimeoutAction action) override;

    // cancel a timer
    bool Cancel(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override 
    {
        return (int)heap_.size();
    }    

private:
    void clear();
    int siftUp(int i);
    void siftDown(int i);
    int delTimer(TimerNode& node);

    std::vector<TimerNode>  heap_;
    std::unordered_map<int, TimerNode> ref_; // O(1) search
};
