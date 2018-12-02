// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerQueueBase.h"
#include <vector>
#include <unordered_map>


// timer queue implemented with priority queue(min-heap)
//
// complexity:
//     AddTimer    CancelTimer   PerTick
//      O(log N)    O(N)          O(1)
//
class PQTimer : public TimerQueueBase
{
public:
    struct TimerNode;

public:
    PQTimer();
    ~PQTimer();

    int Schedule(uint32_t time_units, TimerCallback cb) override;

    bool Cancel(int id) override;

    int Update(int64_t now = 0) override;

    int Size() const override 
    {
        return (int)heap_.size(); 
    }

private:
    // DIY min-heap, you may try std::priority_queue
    void clear();
    bool siftdown(int x, int n);
    void siftup(int j);
    void Cancel(TimerNode* node);

private:
    std::vector<TimerNode*>  heap_;
    std::unordered_map<int, TimerNode*> ref_;
};
