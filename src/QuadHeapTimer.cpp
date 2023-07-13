// Copyright © 2022 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE


#include "QuadHeapTimer.h"
#include "Clock.h"
#include "Logging.h"

using namespace std;

struct TimerNode
{
    int id = 0;       // unique timer id
    int deleted = 0;  // lazy deletion
    int64_t deadline = 0;
    TimeoutAction action = nullptr;
};

QuadHeapTimer::QuadHeapTimer()
{
    timers_.reserve(64); // reserve a little space
}


QuadHeapTimer::~QuadHeapTimer()
{
    clear();
}

void QuadHeapTimer::clear()
{
    for (auto& kv : ref_)
    {
        delete kv.second;
    }
    ref_.clear();
    timers_.clear();
}

// Heap maintenance algorithms.
// details see https://github.com/golang/go/blob/go1.19.10/src/runtime/time.go

// puts the timer at position i in the right place
// in the heap by moving it up toward the top of the heap.
// It returns the smallest changed index.
static int siftupTimer(vector<TimerNode*>& timers, int i)
{
    CHECK(size_t(i) < timers.size());
    int64_t when = timers[i]->deadline;
    CHECK(when > 0);
    TimerNode* tmp = timers[i];
    while (i > 0) {
        int p = (i - 1) / 4; // parent
        if (when >= timers[p]->deadline) {
            break;
        }
        timers[i] = timers[p];
        i = p;
    }
    if (tmp != timers[i]) {
        timers[i] = tmp;
    }
    return i;
}

// puts the timer at position i in the right place
// in the heap by moving it down toward the bottom of the heap.
static void siftdownTimer(vector<TimerNode*>& timers, int i)
{
    int n = int(timers.size());
    CHECK(i < n);
    int64_t when = timers[i]->deadline;
    CHECK(when > 0);
    TimerNode* tmp = timers[i];
    while (true) {
        int c = i * 4 + 1; // left child
        int c3 = c + 2; // mid child
        if (c >= n) {
            break;
        }
        int64_t w = timers[c]->deadline;
        if ((c + 1 < n) && (timers[c + 1]->deadline < w)) {
            w = timers[c + 1]->deadline;
            c++;
        }
        if (c3 < n) {
            int64_t w3 = timers[c3]->deadline;
            if ((c3 + 1 < n) && (timers[c3 + 1]->deadline < w3)) {
                w3 = timers[c3 + 1]->deadline;
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
        timers[i] = timers[c];
        i = c;
    }
    if (tmp != timers[i]) {
        timers[i] = tmp;
    }
}

// removes timer i from the current heap.
// returns the smallest changed index in `timers`
int deltimer(vector<TimerNode*>& timers, int i) {
    int last = int(timers.size()) - 1;
    if (last > 0) {
        timers[0] = timers[last];
    }
    timers[last] = nullptr;
    timers.pop_back();
    int smallestChanged = i;
    if (i != last) {
        // Moving to i may have moved the last timer to a new parent,
        // so sift up to preserve the heap guarantee.
        smallestChanged = siftupTimer(timers, i);
        siftdownTimer(timers, i);
    }
    return smallestChanged;
}

// removes timer 0 from the current heap.
void deltimer0(vector<TimerNode*>& timers) {
    int last = int(timers.size()) - 1;
    if (last > 0) {
        timers[0] = timers[last];
    }
    timers[last] = nullptr;
    timers.pop_back();
    if (last > 0) {
        siftdownTimer(timers, 0);
    }
}

int QuadHeapTimer::Start(uint32_t duration, TimeoutAction action)
{
    int id = nextId();
    int64_t expire = Clock::CurrentTimeMillis() + (int64_t)duration;
    int i = (int)timers_.size();

    TimerNode* node = new TimerNode();
    node->id = id;
    node->deadline = expire;
    node->action = action;

    ref_[id] = node;
    timers_.push_back(node);
    siftupTimer(timers_, i);

    return id;
}

bool QuadHeapTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter != ref_.end()) {
        iter->second->deleted = 1;
        ref_.erase(iter);
        return true;
    }
    return false;
}

int QuadHeapTimer::Tick(int64_t now)
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
        if (node->deleted) {
            deltimer0(timers_);
            ref_.erase(node->id);
            delete node;
            continue;
        }

        auto action = std::move(node->action);

        deltimer0(timers_);
        ref_.erase(node->id);
        delete node;

        fired++;

        if (action) {
            action();
        }
    }
    return fired;
}


