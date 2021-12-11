// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"


class QuadHeapTimer : public TimerBase
{
public:
    struct TimerNode
    {
        int index = -1;
        int id = -1;
        int64_t deadline = 0;
        TimeoutAction action;
    };

public:
    QuadHeapTimer();
    ~QuadHeapTimer();

    int Start(uint32_t time_units, TimeoutAction action) override;

    bool Stop(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override 
    {
        return 0; // TODO:
    }    

private:
    void clear();
    int siftup(int i);
    void siftdown(int i);
    int deltimer(TimerNode& node);

    std::vector<TimerNode>  heap_;
    std::unordered_map<int, TimerNode> ref_; // O(1) search
};
