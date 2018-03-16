// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "WheelTimer.h"
#include "Clock.h"
#include "Logging.h"



WheelTimer::WheelTimer()
    : current_(Clock::GetNowTickCount())
{
}


WheelTimer::~WheelTimer()
{
    clearAll();
}

void WheelTimer::clearList(TimerList* list)
{
    TimerNode* node = list->head.next;
    while (node != nullptr)
    {
        ref_.erase(node->id);
        TimerNode* todel = node;
        node = node->next;
        delete todel;
    }
    list->reset();
}

void WheelTimer::clearAll()
{
    for (int i = 0; i < TVN_SIZE; i++)
    {
        clearList(&near_[i]);
    }
    for (int i = 0; i < WHEEL_BUCKETS; i++)
    {
        for (int j = 0; j < TVR_SIZE; j++)
        {
            clearList(&buckets_[i][j]);
        }
    }
    ref_.clear();
}

void WheelTimer::addTimerNode(TimerNode* node)
{
    int64_t expires = node->expires;
    uint64_t idx = (uint64_t)(expires - jiffies_);
    TimerList* list = nullptr;
    if (idx < TVR_SIZE) // [0, 0x100)
    {     
        int i = expires & TVR_MASK;
        list = &near_[i];
    }
    else if (idx < (1 << (TVR_BITS + TVN_BITS))) // [0x100, 0x4000)
    {
        int i = (expires >> TVR_BITS) & TVN_MASK;
        list = &buckets_[0][i];
    }
    else if (idx < (1 << (TVR_BITS + 2 * TVN_BITS))) // [0x4000, 0x100000)
    {
        int i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
        list = &buckets_[1][i];
    }
    else if (idx < (1 << (TVR_BITS + 3 * TVN_BITS))) // [0x100000, 0x4000000)
    {
        int i = (expires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK;
        list = &buckets_[2][i];
    }
    else if ((int64_t)idx < 0)
    {
        // Can happen if you add a timer with expires == jiffies,
        // or you set a timer to go off in the past
        int i = jiffies_ & TVR_MASK;
        list = &near_[i];
    }
    else
    {
        // If the timeout is larger than MAX_TVAL on 64-bit
        // architectures then we use the maximum timeout
        if (idx > MAX_TVAL)
        {
            idx = MAX_TVAL;
            expires = idx + jiffies_;
        }
        int i = (expires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK;
        list = &buckets_[3][i];
    }
    // add to linked list
    list->tail->next = node;
    list->tail = node;
    list->count++;
}

int WheelTimer::AddTimer(uint32_t time, TimerCallback cb)
{
    TimerNode* node = new TimerNode;
    node->cb = cb;
    node->expires = jiffies_ + time;
    node->id = nextCounter();
    addTimerNode(node);
    ref_[node->id] = node;
    size_++;
    return node->id;
}

// Do lazy cancellation, so single linked list is enough
bool WheelTimer::CancelTimer(int id)
{
    TimerNode* node =(TimerNode*)ref_[id];
    if (node != nullptr)
    {
        node->canceled = true;
        size_--;
        return true;
    }
    return false;
}

// cascade all the timers at bucket of index up one level
bool WheelTimer::cascadeTimers(int bucket, int index)
{
    // swap list
    TimerList list = buckets_[bucket][index];
    buckets_[bucket][index].reset();

    TimerNode* node = list.head.next;
    while (node != nullptr)
    {
        addTimerNode(node);
        node = node->next;
    }
    return index == 0;
}

#define INDEX(N) ((jiffies_ >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

// cascades all vectors and executes all expired timer
void WheelTimer::tick()
{
    int index = jiffies_ & TVR_MASK;
    if (index == 0) // cascade timers
    {
        if (cascadeTimers(0, INDEX(0)) && 
            cascadeTimers(1, INDEX(1)) && 
            cascadeTimers(2, INDEX(2)))
            cascadeTimers(3, INDEX(3));
    }
#undef INDEX
    
    ++jiffies_;

    // swap list
    TimerList expired = near_[index];
    near_[index].reset();

    TimerNode* node = expired.head.next;
    while (node != nullptr)
    {
        if (!node->canceled && node->cb)
        {
            node->cb();
        }
        size_--;
        ref_.erase(node->id);
        TimerNode* todel = node;
        node = node->next;
        delete todel;
    }
}

void WheelTimer::Update()
{
    int64_t now = Clock::GetNowTickCount();
    if (now < current_)
    {
        LOG(ERROR) << "time go backwards: " << now << ", " << current_;
        return;
    }
    else if (now >= current_)
    {
        int64_t diff = (now - current_) / TIME_UNIT;
        current_ = now;
        for (int i = 0; i < diff; i++)
        {
            tick();
        }
    }
}