// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "PQTimer.h"
#include "Clock.h"


PQTimer::PQTimer()
{
    counter_ = 1;
}


PQTimer::~PQTimer()
{
}

int PQTimer::AddTimer(int millsec, TimerCallback cb)
{
    int64_t expire = GetNowTime() + millsec;
    TimerNode* node = new TimerNode;
    node->id = counter_;
    node->canceled = false;
    node->expire = expire;
    node->cb = cb;
    queue_.push(node);
    ref_[counter_] = node;
    counter_++;
    return node->id;
}

void PQTimer::CancelTimer(int id)
{
    TimerNode* node = ref_[id];
    if (node != NULL)
    {
        node->canceled = true;
    }
}

void PQTimer::Tick(int64_t now)
{
    while (!queue_.empty())
    {
        TimerNode* node = queue_.top();
        if (now <= node->expire)
        {
            break;
        }
        queue_.pop();
        ref_.erase(node->id);
        if (!node->canceled && node->cb)
        {
            node->cb();
        }
        delete node;
    }
}
