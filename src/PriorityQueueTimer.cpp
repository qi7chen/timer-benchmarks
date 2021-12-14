// Copyright (C) 2018 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "PriorityQueueTimer.h"
#include "Clock.h"


PriorityQueueTimer::PriorityQueueTimer()
{
    // reserve a little space
    heap_.reserve(16);
}


PriorityQueueTimer::~PriorityQueueTimer()
{
    clear();
}

void PriorityQueueTimer::clear()
{
    heap_.clear();
}

int PriorityQueueTimer::Start(uint32_t ms, TimeoutAction action)
{
    int64_t expire = Clock::CurrentTimeMillis() + ms;
    int i = (int)heap_.size();

    TimerNode node;
    node.id = nextId();
    node.deadline = expire;
    node.action = action;
    node.index = i;

    heap_.push_back(node);
    siftup(i);

    ref_[node.id] = node;
    return node.id;
}

bool PriorityQueueTimer::Stop(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter == ref_.end()) {
        return false;
    }
    deltimer(iter->second);
    return true;
}

void PriorityQueueTimer::deltimer(TimerNode& node)
{
    int n = (int)heap_.size() - 1;
    int i = node.index;
    if (i != n) {
        std::swap(heap_[i], heap_[n]);
        heap_[i].index = i;
    }

    heap_.pop_back();
    ref_.erase(node.id);

    if (i != n) {
        if (!siftdown(i, n)) {
            siftup(i);
        }
    }
}

int PriorityQueueTimer::Tick(int64_t now)
{
    if (heap_.empty()) {
        return 0;
    }
    if (now == 0) {
        now = Clock::CurrentTimeMillis();
    }
    int fired = 0;
    int max_id = next_id_;
    while (!heap_.empty()) {
        TimerNode& node = heap_.front();
        if (now < node.deadline) {
            return fired;
        }
        // make sure we don't process timer created by timer events
        if (node.id > max_id) {
            continue;
        }
        auto action = std::move(node.action);
        deltimer(node);
        fired++;

        if (action) {
            action();
        }
    }
    return fired;
}


bool PriorityQueueTimer::siftdown(int x, int n)
{
    int i = x;
    for (;;)
    {
        int j1 = 2 * i + 1;
        // j1 < 0 after int overflow         
        if ((j1 >= n) || (j1 < 0)) {
            break;
        }
        int j = j1; // left child
        int j2 = j1 + 1;
        if (j2 < n && !(heap_[j1] < heap_[j2])) {
            j = j2; // right child
        }
        if (!(heap_[j] < heap_[i])) {
            break;
        }
        std::swap(heap_[i], heap_[j]);
        heap_[i].index = i;
        heap_[j].index = j;
        i = j;
    }
    return i > x;
}

void PriorityQueueTimer::siftup(int j)
{
    for (;;)
    {
        int i = (j - 1) / 2; // parent node
        if (i == j || !(heap_[j] < heap_[i])) {
            break;
        }
        std::swap(heap_[i], heap_[j]);
        heap_[i].index = i;
        heap_[j].index = j;
        j = i;
    }
}
