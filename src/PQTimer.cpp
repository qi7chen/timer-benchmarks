// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "PQTimer.h"
#include "Clock.h"

struct PQTimer::TimerNode
{
    int index = -1;
    int id = -1;
    int64_t expires = 0;
    TimerCallback cb;

#define HEAP_ITEM_LESS(i, j) (heap_[(i)]->expires < heap_[(j)]->expires)

};

PQTimer::PQTimer()
    : twepoch(Clock::CurrentTimeMillis())
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
    for (auto ptr : heap_)
    {
        delete ptr;
    }
    heap_.clear();
    ref_.clear();
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
        heap_[i]->index = i;
        heap_[j]->index = j;
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
        heap_[i]->index = i;
        heap_[j]->index = j;
        j = i;
    }
}

int PQTimer::AddTimer(uint32_t time, TimerCallback cb)
{
    int64_t expire = Clock::CurrentTimeMillis() - twepoch + time;
    TimerNode* node = new TimerNode;
    node->id = nextCounter();
    node->expires = expire;
    node->cb = cb;
    node->index = heap_.size();
    heap_.push_back(node);
    siftup(heap_.size() - 1);
    ref_[node->id] = node;
    return node->id;
}

bool PQTimer::CancelTimer(int id)
{
    TimerNode* node = (TimerNode*)ref_[id];
    if (node != nullptr)
    {
        int n = (int)heap_.size() - 1;
        int i = node->index;
        if (i != n)
        {
            std::swap(heap_[i], heap_[n]);
            heap_[i]->index = i;
            if (!siftdown(i, n))
            {
                siftup(i);
            }
        }
        heap_.pop_back();
        ref_.erase(id);
        delete node;
        return true;
    }
    return false;
}

void PQTimer::Update()
{
    int64_t now = Clock::CurrentTimeMillis() - twepoch;
    while (!heap_.empty())
    {
        TimerNode* node = heap_.front();
        if (now < node->expires)
        {
            break;
        }
        int n = (int)heap_.size() - 1;
        std::swap(heap_[0], heap_[n]);
        heap_[0]->index = 0;
        siftdown(0, n);
        heap_.pop_back();
        if (node->cb)
        {
            node->cb();
        }
        ref_.erase(node->id);
        delete node;
    }
}
