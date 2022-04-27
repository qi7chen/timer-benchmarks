// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.


#include "QuatHeapTimer.h"
#include "Clock.h"
#include "Logging.h"

QuatHeapTimer::QuatHeapTimer()
{
    // reserve a little space
    heap_.reserve(16);
}


QuatHeapTimer::~QuatHeapTimer()
{
    clear();
}

void QuatHeapTimer::clear()
{
    heap_.clear();
}

int QuatHeapTimer::Start(uint32_t ms, TimeoutAction action)
{
    int id = nextId();
    int64_t expire = Clock::CurrentTimeMillis() + ms;
    int i = (int)heap_.size();

    TimerNode node;
    node.id = id;
    node.deadline = expire;
    node.action = action;
    node.index = i;

    heap_.push_back(node);
    siftUp(i);

    ref_[id] = node;
    return node.id;
}

bool QuatHeapTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter == ref_.end()) {
        return false;
    }
    delTimer(iter->second);
    return true;
}

int QuatHeapTimer::Tick(int64_t now)
{
    if (heap_.empty()) {
        return 0;
    }
    int fired = 0;
    int max_id = next_id_;
    while (!heap_.empty()) {
        TimerNode& node = heap_.front();
        if (now < node.deadline) {
            break; // no more due timer to trigger
        }
        // make sure we don't process newly created timer in timeout event
        if (node.id > max_id) {
            continue;
        }
        auto action = std::move(node.action);
        delTimer(node);
        fired++;

        if (action) {
            action();
        }
    }
    return fired;
}

// removes timer i from the current heap.
// returns the smallest changed index.
int QuatHeapTimer::delTimer(TimerNode& node)
{
    // swap with last element in heap array
    int n = (int)heap_.size() - 1;
    int i = node.index;
    if (i != n) {
        std::swap(heap_[i], heap_[n]);
        heap_[i].index = i;
    }
    
    heap_.pop_back();
    ref_.erase(node.id);

    int smallest = i;
    if (i != n) {
        smallest = siftUp(i);
        siftDown(i);
    }
    return smallest;
}

// Heap maintenance algorithms.
// details see https://github.com/golang/go/blob/go1.16.15/src/runtime/time.go

// `siftUp` puts the timer at position i in the right place
// in the heap by moving it up toward the top of the heap.
// It returns the smallest changed index.
int QuatHeapTimer::siftUp(int i)
{
    CHECK(size_t(i) < heap_.size());
    int64_t when = heap_[i].deadline;
    CHECK(when > 0);
    TimerNode tmp = heap_[i];
    while (i > 0) {
        int p = (i - 1) / 4; // parent
        if (when >= heap_[p].deadline) {
            break;
        }
        heap_[i] = heap_[p];
        heap_[i].index = i;
        i = p;
    }
    if (tmp.id != heap_[i].id) {
        heap_[i] = tmp;
        heap_[i].index = i;
    }
    return i;
}

// `siftDown` puts the timer at position i in the right place
// in the heap by moving it down toward the bottom of the heap.
void QuatHeapTimer::siftDown(int i)
{
    int n = int(heap_.size());
    CHECK(i < n);
    int64_t when = heap_[i].deadline;
    CHECK(when > 0);
    TimerNode tmp = heap_[i];
    while (true) {
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
        heap_[i].index = i;
        i = c;
    }
    if (tmp.id != heap_[i].id) {
        heap_[i] = tmp;
        heap_[i].index = i;
    }
}
