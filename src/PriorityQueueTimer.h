// Copyright (C) 2018 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"
#include <vector>
#include <unordered_map>


// timer queue implemented with priority queue(min-heap)
//
// complexity:
//     AddTimer    CancelTimer   PerTick
//      O(log N)    O(N)          O(1)
//
class PriorityQueueTimer : public TimerBase
{
public:
    struct TimerNode
    {
        int index = -1;
        int id = -1;
        int64_t deadline = 0;
        TimeoutAction action = nullptr;

        bool operator < (const TimerNode& b) const
        {
            return deadline < b.deadline;
        }
    };

public:
    PriorityQueueTimer();
    ~PriorityQueueTimer();

    // start a timer after `ms` milliseconds
    int Start(uint32_t ms, TimeoutAction action) override;

    // stop a timer
    bool Stop(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override 
    {
        return (int)heap_.size(); 
    }

private:
    // DIY min-heap, you may try std::priority_queue
    void clear();
    bool siftdown(int x, int n);
    void siftup(int j);
    void deltimer(TimerNode& node);

private:
    std::vector<TimerNode>  heap_;
    std::unordered_map<int, TimerNode> ref_; // O(1) search
};
