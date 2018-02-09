// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "TreeTimer.h"
#include "Clock.h"

struct TreeTimer::TimerNode
{
    int id = -1;
    int64_t expires = 0;
    TimerCallback cb;

    bool operator < (const TreeTimer::TimerNode& b)
    {
        return expires < b.expires;
    }
};

TreeTimer::TreeTimer()
    : twepoch(CurrentTimeMillis())
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
    tree_.clear();
    ref_.clear();
}

int TreeTimer::AddTimer(uint32_t time, TimerCallback cb)
{
    int64_t expire = CurrentTimeMillis() - twepoch + time;
    TimerNode* node = new TimerNode;
    node->id = ++counter_;
    node->expires = expire;
    node->cb = cb;
    tree_.insert(node);
    ref_[node->id] = node;
    return node->id;
}


bool TreeTimer::CancelTimer(int id)
{
    TimerNode* node = ref_[id];
    if (node != nullptr)
    {
        auto range = tree_.equal_range(node);
        for (auto iter = range.first; iter != range.second; ++iter)
        {
            if ((*iter)->id == id)
            {
                tree_.erase(iter);
                ref_.erase(id);
                delete node;
                return true;
            }
        }
    }
    return false;
}

void TreeTimer::Update()
{
    int64_t now = CurrentTimeMillis() - twepoch;
    while (!tree_.empty())
    {
        auto iter = tree_.begin();
        TimerNode* node = *iter;
        if (now < node->expires)
        {
            break;
        }
        tree_.erase(iter);
        if (node->cb)
        {
            node->cb();
        }
        delete node;
    }
}
