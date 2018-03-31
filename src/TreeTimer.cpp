// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "TreeTimer.h"
#include "Clock.h"


TreeTimer::TreeTimer()
    : twepoch(Clock::CurrentTimeMillis())
{
}


TreeTimer::~TreeTimer()
{
    clear();
}

void TreeTimer::clear()
{
    tree_.clear();
}

int TreeTimer::AddTimer(uint32_t time, TimerCallback cb)
{
    int64_t expire = Clock::CurrentTimeMillis() - twepoch + time;
    TimerNode node;
    node.id = nextCounter();
    node.expires = expire;
    node.cb = cb;
    tree_.insert(node);
    return node.id;
}

// This operation is O(n) complexity
bool TreeTimer::CancelTimer(int id)
{
    for (auto iter = tree_.begin(); iter != tree_.end(); ++iter)
    {
        if (iter->id == id)
        {
            tree_.erase(iter);
            return true;
        }
    }
    return false;
}

void TreeTimer::Update()
{
    int64_t now = Clock::CurrentTimeMillis() - twepoch;
    while (!tree_.empty())
    {
        auto iter = tree_.begin();
        const TimerNode& node = *iter;
        if (now < node.expires)
        {
            break;
        }
        tree_.erase(iter);
        if (node.cb)
        {
            node.cb();
        }
    }
}
