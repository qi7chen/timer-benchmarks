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

int TreeTimer::AddTimer(uint32_t time, TimerCallback cb)
{
    int64_t expire = GetNowTickCount() + time;
    TimerNode* node = new TimerNode;
    node->id = ++counter_;
    node->expires = expire;
    node->cb = cb;
    tree_.insert(node);
    ref_[node->id] = node;
    return node->id;
}


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

void TreeTimer::Update(int64_t now)
{
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