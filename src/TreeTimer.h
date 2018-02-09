// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "ITimer.h"
#include <set>
#include <unordered_map>

// timer queue implemented with red-black tree.
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(log N)   O(log N)      O(log N)
//
class TreeTimer : public ITimerQueue
{
public:
    struct TimerNode;

public:
    TreeTimer();
    ~TreeTimer();

    TreeTimer(const TreeTimer&) = delete;
    TreeTimer& operator=(const TreeTimer&) = delete;

    int AddTimer(uint32_t time, TimerCallback cb) override;

    void CancelTimer(int id) override;

    void Update(int64_t now) override;

private:
    void clear();

private:
    int counter_ = 0;
    std::multiset<TimerNode*> tree_;
    std::unordered_map<int, TimerNode*> ref_;
};

