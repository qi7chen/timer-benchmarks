// Copyright © 2022 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#include "RBTreeTimer.h"
#include "Clock.h"


RBTreeTimer::RBTreeTimer()
{
}


RBTreeTimer::~RBTreeTimer()
{
    clear();
}

void RBTreeTimer::clear()
{
    timers_.clear();
    ref_.clear();
}

int RBTreeTimer::Start(uint32_t duration, TimeoutAction action)
{
    int id = nextId();
    NodeKey key;
    key.id = id;
    key.deadline = Clock::CurrentTimeMillis() + (int64_t)duration;
    timers_.insert(std::make_pair(key, action));
    ref_[id] = key;
    return id;
}

bool RBTreeTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter != ref_.end()) {
        NodeKey key = iter->second;
        ref_.erase(iter);
        timers_.erase(key);
        return true;
    }
    return false;
}

int RBTreeTimer::Update(int64_t now)
{
    if (timers_.size() == 0) {
        return 0;
    }
    auto iter = timers_.begin();
    int fired = 0;
    int max_id = next_id_;
    while (timers_.size() > 0 && iter != timers_.end())
    {
        const NodeKey& key = iter->first;
        if (now < key.deadline) {
            break; // no more due timer to trigger
        }
        // make sure we don't process newly created timer in timeout event
        if (key.id > max_id) {
            break;
        }

        TimeoutAction action = std::move(iter->second);
        ref_.erase(key.id);
        timers_.erase(key);

        if (action) {
            action(); // timeout action
            fired++;
        }

        if (timers_.size() > 0) {
            iter = timers_.begin();
        }
    }
    return fired;
}
