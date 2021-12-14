// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HashedWheelBucket.h"
#include "HashedWheelTimer.h"
#include "Logging.h"
#include <assert.h>


bool HashedWheelTimeout::Expire()
{
    // trigger timeout action
    if (action) {
        action();
        action = nullptr;
        return true;
    }
    return false;
}

void HashedWheelTimeout::Remove()
{
    if (bucket != nullptr) {
        bucket->Remove(this);
    }
}

HashedWheelBucket::HashedWheelBucket()
{
}

HashedWheelBucket::~HashedWheelBucket()
{
}

// add `timeout` to this bucket
void HashedWheelBucket::AddTimeout(HashedWheelTimeout* timeout)
{
    CHECK(timeout != nullptr);
    CHECK(timeout->bucket == nullptr);
    timeout->bucket = this;
    if (head == nullptr) {
        head = tail = timeout;
    }
    else {
        tail->next = timeout;
        timeout->prev = tail;
        tail = timeout;
    }
}

// Expire all HashedWheelTimeouts for the given deadline.
void HashedWheelBucket::ExpireTimeouts(int64_t deadline, std::vector<HashedWheelTimeout*>& expired)
{
    HashedWheelTimeout* timeout = head;
    while (timeout != nullptr) {
        HashedWheelTimeout* next = timeout->next;
        if (timeout->remaining_rounds <= 0) {
            next = Remove(timeout);
            if (timeout->deadline <= deadline) {
                expired.push_back(timeout);
            }
            else {
                // The timeout was placed into a wrong slot. This should never happen.
                LOG(FATAL) << "timeout deadline";
            }
        }
        else {
            timeout->remaining_rounds--;
        }
        timeout = next;
    }
}

// remove `timeout` from linked list and return next linked one
HashedWheelTimeout* HashedWheelBucket::Remove(HashedWheelTimeout* timeout)
{
    HashedWheelTimeout* next = timeout->next;
    if (timeout->prev != nullptr) {
        timeout->prev->next = next;
    }
    if (timeout->next != nullptr) {
        timeout->next->prev = timeout->prev;
    }
    if (timeout == head) {
        // if timeout is also the tail we need to adjust the entry too
        if (timeout == tail) {
            head = tail = nullptr;
        }
        else {
            head = next;
        }
    }
    else if (timeout == tail) {
        // if the timeout is the tail modify the tail to be the prev node.
        tail = timeout->prev;
    }
    // unchain from this bucket
    timeout->prev = nullptr;
    timeout->next = nullptr;
    timeout->bucket = nullptr;
    return next;
}

// Clear this bucket and return all not expired / cancelled Timeouts.
void HashedWheelBucket::ClearTimeouts(std::unordered_map<int, HashedWheelTimeout*>& set)
{
    while (true)
    {
        HashedWheelTimeout* timeout = pollTimeout();
        if (timeout == nullptr) {
            break;
        }
        set[timeout->id] = timeout;
    }
}

// poll first timeout
HashedWheelTimeout* HashedWheelBucket::pollTimeout()
{
    HashedWheelTimeout* node = this->head;
    if (node == nullptr) {
        return nullptr;
    }
    HashedWheelTimeout* next = node->next;
    if (next == nullptr) {
        head = tail = nullptr;
    }
    else {
        head = next;
        next->prev = nullptr;
    }
    // unchain this element
    node->next = nullptr;
    node->prev = nullptr;
    node->bucket = nullptr;
    return node;
}