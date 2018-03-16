// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerQueueBase.h"
#include <set>


// timer queue implemented with red-black tree.
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(log N)   O(log N)      O(log N)
//
class TreeTimer : public TimerQueueBase
{
public:
    struct TimerNode;

public:
    TreeTimer();
    ~TreeTimer();

    int AddTimer(uint32_t time, TimerCallback cb) override;

    bool CancelTimer(int id) override;

    void Update() override;

    int Size() const override 
    { 
        return (int)tree_.size(); 
    }

private:
    void clear();

private:
    const int64_t twepoch;                  // custom epoch
    std::multiset<TimerNode*>   tree_;      // std::multiset as a red-black tree
};

