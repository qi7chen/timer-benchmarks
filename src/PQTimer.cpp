// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "PQTimer.h"
#include "Clock.h"

#define HEAP_ITEM_LESS(i, j) (heap_[(i)].when < heap_[(j)].when)

PQTimer::PQTimer()
{
    // reserve a little space
    heap_.reserve(64);
}


PQTimer::~PQTimer()
{
    clear();
}

void PQTimer::clear()
{
    heap_.clear();
}

bool PQTimer::siftdown(int x, int n)
{
    int i = x;
    for (;;)
    {
        int j1 = 2 * i + 1;
        if ((j1 >= n) || (j1 < 0)) // j1 < 0 after int overflow
        {
            break;
        }
        int j = j1; // left child
        int j2 = j1 + 1;
        if (j2 < n && !HEAP_ITEM_LESS(j1, j2))
        {
            j = j2; // right child
        }
        if (!HEAP_ITEM_LESS(j, i))
        {
            break;
        }
        std::swap(heap_[i], heap_[j]);
        heap_[i].index = i;
        heap_[j].index = j;
        i = j;
    }
    return i > x;
}

void PQTimer::siftup(int j)
{
    for (;;)
    {
        int i = (j - 1) / 2; // parent node
        if (i == j || !HEAP_ITEM_LESS(j, i))
        {
            break;
        }
        std::swap(heap_[i], heap_[j]);
        heap_[i].index = i;
        heap_[j].index = j;
        j = i;
    }
}

int PQTimer::Schedule(uint32_t time_units, TimerCallback cb)
{
    int64_t expire = Clock::CurrentTimeUnits() + time_units;
    TimerNode node;
    int id = nextCounter();
    node.id = id;
    node.when = expire;
    node.cb = cb;
    node.index = (int)heap_.size();
    heap_.push_back(node);
    siftup((int)heap_.size() - 1);
    ref_[id] = node;
    return id;
}

bool PQTimer::Cancel(int id)
{
    auto iter = ref_.find(id);
    if (iter == ref_.end())
    {
        return false;
    }
    Cancel(iter->second);
    return true;
}

void PQTimer::Cancel(TimerNode& node)
{
    int n = (int)heap_.size() - 1;
    int i = node.index;
    if (i != n)
    {
        std::swap(heap_[i], heap_[n]);
        heap_[i].index = i;
        if (!siftdown(i, n))
        {
            siftup(i);
        }
    }
    heap_.pop_back();
    ref_.erase(node.id);
}

int PQTimer::Update(int64_t now)
{
    if (heap_.empty())
    {
        return 0;
    }
    if (now == 0)
    {
        now = Clock::CurrentTimeUnits();
    }
    int fired = 0;
    while (!heap_.empty())
    {
        TimerNode& node = heap_.front();
        if (now < node.expires)
        {
            break;
        }
        
        auto cb = std::move(node.cb);
        Cancel(node);
        fired++;

        if (cb)
        {
            cb();
        }
    }
    return fired;
}
