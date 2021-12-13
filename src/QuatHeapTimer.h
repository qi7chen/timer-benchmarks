// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"


// Quaternary-ary heap
// https://en.wikipedia.org/wiki/D-ary_heap
class QuatHeapTimer : public TimerBase
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
    QuatHeapTimer();
    ~QuatHeapTimer();

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
    void clear();
    int siftup(int i);
    void siftdown(int i);
    int deltimer(TimerNode& node);

    std::vector<TimerNode>  heap_;
    std::unordered_map<int, TimerNode> ref_; // O(1) search
};
