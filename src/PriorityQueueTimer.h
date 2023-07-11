// Copyright (C) 2021 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"
#include <vector>
#include <unordered_map>


// timer scheduler implemented by priority queue(min-heap)
//
// complexity:
//     StartTimer  CancelTimer   PerTick
//      O(log N)    O(N)          O(1)
//
class PriorityQueueTimer : public TimerBase
{
public:
    struct TimerNode
    {
        int index = -1;  // array index at heap
        int id = 0;      // unique timer id
        int64_t deadline = 0;   // expired time in ms
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
    PriorityQueueTimer();
    ~PriorityQueueTimer();

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
    // do our own min-heap algorithm to smoothing differences between different STL implementations,
    // you may replace this with std::priority_queue instead
    void clear();
    bool siftDown(int x, int n);
    void siftUp(int j);
    void delTimer(TimerNode& node);

private:
    std::vector<TimerNode>  heap_; // timer heap

    std::unordered_map<int, TimerNode> ref_; // a hashmap to make O(1) lookup
};
