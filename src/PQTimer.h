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
    struct TimerNode
    {
        int index = -1;
        int id = -1;
        int64_t expires = 0;
        TimerCallback cb;
    };

public:
    PQTimer();
    ~PQTimer();

    int RunAfter(uint32_t milliseconds, TimerCallback cb) override;

    bool Cancel(int id) override;

    int Update() override;

    int Size() const override 
    {
        return (int)heap_.size(); 
    }

private:
    // DIY min-heap, you may try std::priority_queue
    void clear();
    bool siftdown(int x, int n);
    void siftup(int j);

private:
    std::vector<TimerNode>  heap_;
};
