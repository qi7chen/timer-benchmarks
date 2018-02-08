// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "itimer.h"
#include <queue>
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
        int64_t     expire;
        int         id;
        bool        canceled;
        TimerCallback cb;
        

        bool operator < (const PQTimer::TimerNode& b)
        {
            return expire < b.expire;
        }
    };

public:
    PQTimer();
    ~PQTimer();

    int AddTimer(int millsec, TimerCallback cb) override;

    void CancelTimer(int id) override;

    void Tick(int64_t now) override;

private:
    int counter_;
    std::priority_queue<TimerNode*> queue_;
    std::unordered_map<int, TimerNode*> ref_;
};
