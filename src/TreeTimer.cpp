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
    for (auto ptr : tree_)
    {
        delete ptr;
    }
}

int TreeTimer::AddTimer(int millsec, TimerCallback cb)
{
    int64_t expire = GetNowTime() + millsec;
    TimerNode* node = new TimerNode;
    node->id = counter_;
    node->expire = expire;
    node->cb = cb;
    tree_.insert(node);
    ref_[counter_] = node;
    counter_++;
    return node->id;
}

//  Complexity of this operation is O(n). We assume it is rarely used.
void TreeTimer::CancelTimer(int id)
{
    TimerNode* node = ref_[id];
    if (node != nullptr)
    {
        auto range = tree_.equal_range(node);
        for (auto iter = range.first; iter != range.second; ++iter)
        {
            if ((*iter)->id == id)
            {
                delete *iter;
                tree_.erase(iter);
                ref_.erase(id);
                break;
            }
        }
    }
}

void TreeTimer::Tick(int64_t now)
{
    while (!tree_.empty())
    {
        auto iter = tree_.begin();
        TimerNode* node = *iter;
        if (now > node->expire)
        {
            tree_.erase(iter);
            if (node->cb)
            {
                node->cb();
            }
            delete node;
        }
    }
}