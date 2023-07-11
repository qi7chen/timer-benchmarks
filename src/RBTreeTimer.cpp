// Copyright (C) 2022 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

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
}

int RBTreeTimer::Start(uint32_t ms, TimeoutAction action)
{
    int id = nextId();
    NodeKey key;
    key.id = id;
    key.deadline = Clock::CurrentTimeMillis() + ms;
    tree_.put(key, action);
    ref_[id] = key;
    return id;
}

bool RBTreeTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter == ref_.end()) {
        return false;
    }
    tree_.remove(iter->second);
    ref_.erase(timer_id);
    return true;
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
    
    int max_id = next_id_;
    std::vector<NodeKey> expired; // expired keys to delete
    while (entry != nullptr)
    {
        if (now < entry->key.deadline) {
            break; // no more due timer to trigger
        }
        // make sure we don't process newly created timer in timeout event
        if (entry->key.id > max_id) {
            continue;
        }
        expired.push_back(entry->key);
        entry = entry->next();
    }
    int fired = (int)expired.size();
    for (int i = 0; i < fired; i++) {
        auto entry = tree_.getEntry(expired[i]);
        if (entry != nullptr) {
            TimeoutAction action = std::move(entry->value);
            ref_.erase(entry->key.id);
            tree_.remove(entry->key);
            if (action) {
                action(); // timeout action
            }
        }
    }
    return fired;
}
