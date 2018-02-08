// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "PQTimer.h"
#include "Clock.h"


PQTimer::PQTimer()
{
    ref_.rehash(16);
    heap_.reserve(16);
#define HEAP_ITEM_LESS(a, b) (heap_[(a)]->expire < heap_[(b)]->expire)
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

int PQTimer::AddTimer(int millsec, TimerCallback cb)
{
    int64_t expire = GetNowTime() + millsec;
    TimerNode* node = new TimerNode;
    node->id = counter_;
    node->expire = expire;
    node->cb = cb;
    heap_.push_back(node);
    siftup(heap_.size() - 1);
    ref_[counter_] = node;
    counter_++;
    return node->id;
}

//  Complexity of this operation is O(n). We assume it is rarely used.
void PQTimer::CancelTimer(int id)
{
    TimerNode* node = ref_[id];
    if (node != nullptr)
    {
        int n = (int)heap_.size() - 1;
        int i = node->index;
        if (i != n)
        {
            std::swap(heap_[i], heap_[n]);
            if (!siftdown(i, n))
            {
                siftup(i);
            }
        }
        heap_.pop_back();
        ref_.erase(id);
    }
}

void PQTimer::Tick(int64_t now)
{
    while (!heap_.empty())
    {
        TimerNode* node = heap_.front();
        if (now > node->expire)
        {
            int n = (int)heap_.size() - 1;
            std::swap(heap_[0], heap_[n]);
            heap_[n]->index = 0;
            siftdown(0, n);
            heap_.pop_back();
            if (node->cb)
            {
                node->cb();
            }
            delete node;
        }
    }
}
