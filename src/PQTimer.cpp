// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "PQTimer.h"
#include "Clock.h"

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

#define HEAP_ITEM_LESS(i, j) (heap_[(i)].expires < heap_[(j)].expires)

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

int PQTimer::AddTimer(uint32_t time, TimerCallback cb)
{
    int64_t expire = Clock::CurrentTimeMillis() + time;
    TimerNode node;
    node.id = nextCounter();
    node.expires = expire;
    node.cb = cb;
    node.index = (int)heap_.size();
    heap_.push_back(node);
    siftup((int)heap_.size() - 1);
    return node.id;
}

// This operation is O(n) complexity
bool PQTimer::CancelTimer(int id)
{
    for (int idx = 0; idx < (int)heap_.size(); idx++)
    {
        if (heap_[idx].id == id)
        {
            int n = (int)heap_.size() - 1;
            int i = heap_[idx].index;
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
            return true;
        }
    }
    return false;
}

void PQTimer::Update()
{
    int64_t now = Clock::CurrentTimeMillis();
    while (!heap_.empty())
    {
        TimerNode& node = heap_.front();
        if (now < node.expires)
        {
            break;
        }
        auto cb = std::move(node.cb);
        int n = (int)heap_.size() - 1;
        std::swap(heap_[0], heap_[n]);
        heap_[0].index = 0;
        siftdown(0, n);
        heap_.pop_back();

        if (cb)
        {
            cb();
        }
    }
}
