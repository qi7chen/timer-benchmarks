// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "TreeTimer.h"
#include "Clock.h"


TreeTimer::TreeTimer()
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

int TreeTimer::RunAfter(uint32_t milsec, TimerCallback cb)
{
    int64_t expire = Clock::CurrentTimeMillis() + milsec;
    TimerNode node;
    node.id = nextCounter();
    node.expires = expire;
    node.cb = cb;
    tree_.insert(node);
    return node.id;
}

// This operation is O(n) complexity
bool TreeTimer::Cancel(int id)
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

int TreeTimer::Update(int64_t now)
{
    if (tree_.empty())
    {
        return 0;
    }
    int fired = 0;
    if (now == 0)
    {
        now = Clock::CurrentTimeMillis();
    }
    while (!tree_.empty())
    {
        auto iter = tree_.begin();
        const TimerNode& node = *iter;
        if (now < node.expires)
        {
            break;
        }
        auto cb = std::move(node.cb);
        tree_.erase(iter);
        fired++;
        if (cb)
        {
            cb();
        }
    }
    return fired;
}
