// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"
#include "RBTree.h"
#include <unordered_map>

// timer queue implemented with red-black tree.
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(log N)   O(N)          O(log N)
//
class RBTreeTimer : public TimerBase
{
public:
    struct NodeKey
    {
        int id = -1;
        int64_t deadline = 0;
        
        bool operator < (const NodeKey& b) const
        {
            if (deadline == b.deadline) {
                return id > b.id; // bigger id is later added
            }
            return deadline < b.deadline;
        }
    };

public:
    RBTreeTimer();
    ~RBTreeTimer();

    // start a timer after `ms` milliseconds
    int Start(uint32_t ms, TimeoutAction action) override;

    // stop a timer
    bool Stop(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override 
    { 
        return timers_.Size();
    }

private:
    void clear();

    std::unordered_map<int, NodeKey> ref_;
    RBTree<NodeKey, TimeoutAction> timers_;
};

