// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HHWheel.h"
#include "Logging.h"
#include "Clock.h"


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

void WheelTimerBucket::Clear()
{
    WheelTimerNode* node = head;
    while (node != nullptr)
    {
        WheelTimerNode* next = node->next;
        delete node;
        node = next;
    }
    head = nullptr;
    tail = nullptr;
}


HHTimingWheel::HHTimingWheel()
    : started_at_(Clock::CurrentTimeMillis())
{
}

HHTimingWheel::~HHTimingWheel()
{
    Clear();
}

bool HHTimingWheel::AddNode(WheelTimerNode* node)
{
    CHECK(node->bucket == nullptr);
    int64_t ticks = (node->deadline - started_at_) / TIME_UNIT;
    WheelTimerBucket* bucket = nullptr;
    if (ticks < TVR_SIZE) 
    {
        int i = ticks & TVR_MASK;
        bucket = &near_[i];
    }
    else 
    {
        for (int i = 0; i < 4; i++)
        {
            int64_t size = 1ULL << (TVR_BITS + (i + 1) * TVN_BITS);
            if (ticks < size) 
            {
                int idx = (ticks >> (TVR_BITS + (i)*TVN_BITS)) & TVN_MASK;
                bucket = &buckets_[i][idx];
            }
        }
    }
    bucket->AddNode(node);
    return true;
}


void HHTimingWheel::Remove(WheelTimerNode* node)
{
    CHECK(node != nullptr);
    CHECK(node->bucket != NULL);
    node->bucket->Remove(node);
}

void HHTimingWheel::Clear()
{
    for (int i = 0; i < TVR_SIZE; i++)
    {
        near_[i].Clear();
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < TVN_SIZE; j++)
        {
            buckets_[i][j].Clear();
        }
    }
}

bool HHTimingWheel::Cascade(int level, int index)
{
    WheelTimerNode* node = buckets_[level][index].Splice();
    while (node != nullptr)
    {
        WheelTimerNode* next = node;
        node->prev = nullptr;
        node->next = nullptr;
        node->bucket = nullptr;
        AddNode(node);
        node = next;
    }
    return index == 0;
}

int HHTimingWheel::ExpireNear()
{
    int count = 0;
    int idx = currtick_ & TVR_MASK;
    WheelTimerBucket* bucket = &near_[idx];
    WheelTimerNode* node = bucket->Splice();
    while (node != nullptr)
    {
        WheelTimerNode* next = node->next;
        node->Expire();
        count++;
        node = next;
    }
    return count;
}

void HHTimingWheel::ShiftLevel()
{
    if (currtick_ == 0) 
    {
        Cascade(3, 0);
        return;
    }
    int mask = TVR_SIZE;
    uint32_t tz = currtick_ >> TVR_BITS;
    int i = 0;
    while ((currtick_ & (mask - 1)) == 0)
    {
        int idx = tz & TVN_MASK;
        if (idx != 0)
        {
            Cascade(i, idx);
            break;
        }
        mask <<= TVN_BITS;
        tz >>= TVN_BITS;
        i++;
    }
}

// implementation inspired by skynet_timer.c
// see https://github.com/cloudwu/skynet/blob/v1.5.0/skynet-src/skynet_timer.c
int HHTimingWheel::Tick()
{
    int count = 0;
    count += ExpireNear(); // try to dispatch timeout 0 (rare condition)
    currtick_++;
    ShiftLevel();
    count += ExpireNear();
    return count;
}
