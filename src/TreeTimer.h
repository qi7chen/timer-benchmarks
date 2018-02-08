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
//       O(lgN)      O(N)         O(lgN)
//
class TreeTimer : public ITimerQueue
{
public:
    struct TimerNode
    {
        int64_t expire = 0;
        int id = -1;
        TimerCallback cb;

        bool operator < (const TreeTimer::TimerNode& b)
        {
            return expire < b.expire;
        }
    };

public:
    TreeTimer();
    ~TreeTimer();

    TreeTimer(const TreeTimer&) = delete;
    TreeTimer& operator=(const TreeTimer&) = delete;

    int AddTimer(int millsec, TimerCallback cb) override;

    void CancelTimer(int id) override;

    void Tick(int64_t now) override;

private:
    void clear();

private:
    int counter_ = 1;
    std::multiset<TimerNode*> tree_;
    std::unordered_map<int, TimerNode*> ref_;
};

