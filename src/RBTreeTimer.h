// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"
#include "RBTree.h"


// timer queue implemented with red-black tree.
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(log N)   O(N)          O(log N)
//
class RBTreeTimer : public TimerBase
{
public:
    struct TimerNode
    {
        int id = -1;
        int64_t deadline = 0;
        TimeoutAction action = nullptr;

        bool operator < (const TimerNode& b) const
        {
            return deadline < b.deadline;
        }
    };

    typedef std::list<TimerNode*> TimerNodeList;

public:
    RBTreeTimer();
    ~RBTreeTimer();

    int Start(uint32_t time_units, TimeoutAction action) override;

    bool Stop(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override 
    { 
        return size_;
    }

private:
    void clear();

    int size_ = 0;
    RBTree<int64_t, TimerNode*> timers_;
};

