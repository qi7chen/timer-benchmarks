// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "ITimer.h"
#include <unordered_map>


// timer queue implemented with min-heap
//
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(lgN)     O(1)           O(1)
//
class PQTimer : public ITimerQueue
{
public:
    struct TimerNode
    {
        int index = -1;
        int id = -1;
        int64_t expire = 0;
        TimerCallback cb;

        bool operator < (const PQTimer::TimerNode& b)
        {
            return expire > b.expire; // we need min-heap
        }
    };

public:
    PQTimer();
    ~PQTimer();

    PQTimer(const PQTimer&) = delete;
    PQTimer& operator=(const PQTimer&) = delete;

    int AddTimer(int millsec, TimerCallback cb) override;

    void CancelTimer(int id) override;

    void Tick(int64_t now) override;

private:
    void clear();
    bool siftdown(int x, int n);
    void siftup(int j);

private:
    int counter_ = 1;
    std::vector<TimerNode*> heap_;
    std::unordered_map<int, TimerNode*> ref_;
};
