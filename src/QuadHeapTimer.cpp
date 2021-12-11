// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.


#include "QuadHeapTimer.h"
#include "Clock.h"
#include <assert.h>

QuadHeapTimer::QuadHeapTimer()
{
    // reserve a little space
    heap_.reserve(16);
}


QuadHeapTimer::~QuadHeapTimer()
{
    clear();
}

void QuadHeapTimer::clear()
{
    heap_.clear();
}

int QuadHeapTimer::Start(uint32_t time_units, TimeoutAction action)
{
    int64_t expire = Clock::CurrentTimeUnits() + time_units;
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

bool QuadHeapTimer::Stop(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter == ref_.end()) {
        return false;
    }
    deltimer(iter->second);
    return true;
}

int QuadHeapTimer::Tick(int64_t now)
{
    if (heap_.empty()) {
        return 0;
    }
    if (now == 0) {
        now = Clock::CurrentTimeUnits();
    }
    int fired = 0;
    int max_id = next_id_;
    while (!heap_.empty()) {
        TimerNode& node = heap_.front();
        if (now < node.deadline)
        {
            break;
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

// removes timer i from the current heap.
// returns the smallest changed index.
int  QuadHeapTimer::deltimer(TimerNode& node)
{
    int n = (int)heap_.size() - 1; // last node
    int i = node.index;
    if (i != n) {
        heap_[i] = heap_[n];
        heap_[i].index = i;
    }
    heap_.pop_back();
    ref_.erase(node.id);
    int smallest = i;
    if (i != n) {
        smallest = siftup(i);
        siftdown(i);
    }
    return smallest;
}

// `siftup` puts the timer at position i in the right place
// in the heap by moving it up toward the top of the heap.
// It returns the smallest changed index.
int QuadHeapTimer::siftup(int i)
{
    assert(size_t(i) < heap_.size());
    int64_t when = heap_[i].deadline;
    TimerNode tmp = heap_[i];
    while (i > 0) {
        int p = (i - 1) / 4; // parent
        if (when >= heap_[p].deadline) {
            break;
        }
        heap_[i] = heap_[p];
        i = p;
    }
    if (tmp.id != heap_[i].id) {
        heap_[i] = tmp;
    }
    return i;
}

// `siftdown` puts the timer at position i in the right place
// in the heap by moving it down toward the bottom of the heap.
void QuadHeapTimer::siftdown(int i)
{
    int n = int(heap_.size());
    assert(i < n);
    int64_t when = heap_[i].deadline;
    TimerNode tmp = heap_[i];
    while (true)
    {
        int c = i * 4 + 1; // left child
        int c3 = c + 2; // mid child
        if (c >= n) {
            break;
        }
        int64_t w = heap_[c].deadline;
        if ((c + 1 < n) && (heap_[c + 1].deadline < w)) {
            w = heap_[c + 1].deadline;
            c++;
        }
        if (c3 < n) {
            int64_t w3 = heap_[c3].deadline;
            if ((c3 + 1 < n) && (heap_[c3 + 1].deadline < w3)) {
                w3 = heap_[c3 + 1].deadline;
                c3++;
            }
            if (w3 < w) {
                w = w3;
                c = c3;
            }
        }
        if (w >= when) {
            break;
        }
        heap_[i] = heap_[c];
        i = c;
    }
    if (tmp.id != heap_[i].id) {
        heap_[i] = tmp;
    }
}
