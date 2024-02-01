// Copyright © 2021 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#include "PriorityQueueTimer.h"
#include "Clock.h"

using namespace std;

struct TimerNode
{
    int index = -1;  // array index at heap
    int id = 0;      // unique timer id
    int64_t deadline = 0;   // expired time in ms
    TimeoutAction action = nullptr;

    bool lessThan(const TimerNode* b) const
    {
        if (deadline == b->deadline) {
            return id > b->id;
        }
        return deadline < b->deadline;
    }
};


PriorityQueueTimer::PriorityQueueTimer()
{
    timers_.reserve(64); // reserve a little space
}


PriorityQueueTimer::~PriorityQueueTimer()
{
    clear();
}

void PriorityQueueTimer::clear()
{
    for (auto& kv : ref_)
    {
        delete(kv.second);
    }
    ref_.clear();
    timers_.clear();
}

// Heap maintenance algorithms.
// this to ensure run same min-heap algorithm on different platform,
// you may replace with std::priority_queue instead

static bool siftdownTimer(vector<TimerNode*>& timers, int x, int n)
{
    int i = x;
    for (;;) {
        int j1 = 2 * i + 1;
        // j1 < 0 after int overflow         
        if ((j1 >= n) || (j1 < 0)) {
            break;
        }
        int j = j1; // left child
        int j2 = j1 + 1;
        if (j2 < n && !(timers[j1] < timers[j2])) {
            j = j2; // = 2*i + 2right child
        }
        if (!(timers[j]->lessThan(timers[i]))) {
            break;
        }
        std::swap(timers[i], timers[j]);
        timers[i]->index = i;
        timers[j]->index = j;
        i = j;
    }
    return i > x;
}

static void siftupTimer(vector<TimerNode*>& timers, int j)
{
    for (;;) {
        int i = (j - 1) / 2; // parent node
        if (i == j || !(timers[j] < timers[i])) {
            break;
        }
        std::swap(timers[i], timers[j]);
        timers[i]->index = i;
        timers[j]->index = j;
        j = i;
    }
}

static void removeTimer(vector<TimerNode*>& timers, int i) {
    // swap with last element of array
    int n = (int)timers.size() - 1;
    if (i != n) {
        std::swap(timers[i], timers[n]);
        timers[i]->index = i;

        // re-balance heap
        if (!siftdownTimer(timers, i, n)) {
            siftupTimer(timers, i);
        }
    }
    timers.pop_back();
}

int PriorityQueueTimer::Start(uint32_t duration, TimeoutAction action)
{
    int id = nextId();
    int64_t expire = Clock::CurrentTimeMillis() + (int64_t)duration;
    int i = (int)timers_.size();

    TimerNode* node = new TimerNode;
    node->id = id;
    node->index = i;
    node->deadline = expire;
    node->action = action;

    ref_[id] = node;
    timers_.push_back(node);
    siftupTimer(timers_, i);

    return id;
}

bool PriorityQueueTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter != ref_.end()) {
        TimerNode* node = iter->second;
        removeTimer(timers_, node->index);
        ref_.erase(iter);
        delete node;
        return true;
    }
    return false;
}

int PriorityQueueTimer::Update(int64_t now)
{
    if (timers_.empty()) {
        return 0;
    }
    int fired = 0;
    int max_id = next_id_;
    while (!timers_.empty()) {
        TimerNode* node = timers_[0];
        if (now < node->deadline) {
            break; // no timer expired
        }
        if (node->id > max_id) {
            break; // process newly added timer at next tick
        }
        auto action = std::move(node->action);

        removeTimer(timers_, 0);
        ref_.erase(node->id);
        delete node;

        fired++;

        if (action) {
            action();
        }
    }
    return fired;
}

