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
    tree_.clear();
    ref_.clear();
}

int RBTreeTimer::Start(uint32_t duration, TimeoutAction action)
{
    int id = nextId();
    NodeKey key;
    key.id = id;
    key.deadline = Clock::CurrentTimeMillis() + (int64_t)duration;
    tree_.put(key, action);
    ref_[id] = key;
    return id;
}

bool RBTreeTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter != ref_.end()) {
        tree_.remove(iter->second);
        ref_.erase(iter);
        return true;
    }
    return false;
}

int RBTreeTimer::Tick(int64_t now)
{
    if (tree_.size() == 0) {
        return 0;
    }
    auto entry = tree_.getFirstEntry();
    if (entry == nullptr) {
        return 0;
    }
    int fired = 0;
    int max_id = next_id_;
    while (entry != nullptr)
    {
        if (now < entry->key.deadline) {
            break; // no more due timer to trigger
        }
        // make sure we don't process newly created timer in timeout event
        if (entry->key.id > max_id) {
            break;
        }

        TimeoutAction action = std::move(entry->value);
        ref_.erase(entry->key.id);
        tree_.remove(entry->key);
        if (action) {
            action(); // timeout action
        }

        entry = tree_.getFirstEntry();
        fired++;
    }
    return fired;
}
