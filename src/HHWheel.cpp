// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HHWheel.h"
#include "Logging.h"
#include "Clock.h"
#include "HHWheelTimer.h"


void WheelTimerNode::Expire()
{
    if (action != nullptr) 
    {
        action();
        action = nullptr;
    }
}

void WheelTimerNode::Remove()
{
    CHECK(bucket != nullptr);
    bucket->Remove(this);
}


void WheelTimerBucket::AddNode(WheelTimerNode* node)
{
    CHECK(node != nullptr);
    CHECK(node->bucket == nullptr);
    node->bucket = this;
    if (head == nullptr) {
        head = tail = node;
    }
    else {
        tail->next = node;
        node->prev = tail;
        tail = node;
    }
}

WheelTimerNode* WheelTimerBucket::Remove(WheelTimerNode* node)
{
    WheelTimerNode* next = node->next;
    if (node->prev != nullptr) {
        node->prev->next = next;
    }
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    }
    if (node == head) {
        // if timeout is also the tail we need to adjust the entry too
        if (node == tail) {
            head = tail = nullptr;
        }
        else {
            head = next;
        }
    }
    else if (node == tail) {
        // if the timeout is the tail modify the tail to be the prev node.
        tail = node->prev;
    }
    // unchain from this bucket
    node->prev = nullptr;
    node->next = nullptr;
    node->bucket = nullptr;
    return next;
}

WheelTimerNode* WheelTimerBucket::Splice()
{
    WheelTimerNode* node = head;
    head = nullptr;
    tail = nullptr;
    return node;
}
