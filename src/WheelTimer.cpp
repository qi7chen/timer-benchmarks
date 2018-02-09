// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "WheelTimer.h"
#include "Clock.h"
#include "Logging.h"

const int64_t TIME_UNIT = int64_t(1e7);       // centisecond, i.e. 1/100 second
const uint64_t MAX_TVAL = ((uint64_t)((1ULL << (TVR_BITS + 4 * TVN_BITS)) - 1));

WheelTimer::WheelTimer()
{
    current_ = GetNowTickCount();
    time_point_ = GetNowTime();
    ref_.rehash(32);
}


WheelTimer::~WheelTimer()
{
    clearAll();
}

void WheelTimer::clearAll()
{

}

void WheelTimer::addTimerNode(TimerNode* node)
{
    int64_t expires = node->expires;
    int64_t idx = expires - jiffies_;
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
    else if (idx < 0)
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
}

int WheelTimer::AddTimer(uint32_t time, TimerCallback cb)
{
    TimerNode* node = new TimerNode;
    node->cb = cb;
    node->expires = jiffies_ + time;
    node->id = ++counter_;
    addTimerNode(node);
    ref_[node->id] = node;
    return node->id;
}

// Do lazy cancellation, so single linked list is enough
void WheelTimer::CancelTimer(int id)
{
    TimerNode* node = ref_[id];
    if (node != nullptr)
    {
        node->canceld = true;
    }
}

void WheelTimer::cascadeTimer(int bucket, int index)
{
    TimerList* list = &buckets_[bucket][index];
    TimerNode* node = list->head.next;
    while (node != nullptr)
    {
        addTimerNode(node);
        node = node->next;
    }
    list->reset();
}

// cascades all vectors and executes all expired timer
void WheelTimer::tick()
{
    int index = jiffies_ & TVR_MASK;
    if (index == 0)
    {
        // cascade timers
        for (int i = 0; i < WHEEL_BUCKETS; i++)
        {
            int idx = ((jiffies_ >> (TVR_BITS + (i)* TVN_BITS)) & TVN_MASK);
            cascadeTimer(i, idx);
            if (idx > 0)
            {
                break;
            }
        }
    }
    
    ++jiffies_;

    TimerList* expired = &near_[index];
    TimerNode* node = expired->head.next;
    while (node != nullptr)
    {
        if (!node->canceld && node->cb)
        {
            node->cb();
        }
        ref_.erase(node->id);
        delete node;
        node = node->next;
    }
    expired->reset();
}

void WheelTimer::Update(int64_t now)
{
    if (now < current_)
    {
        LOG(ERROR) << "time go backwards: " << now << ", " << current_;
        return;
    }
    else if (now >= current_)
    {
        int64_t diff = now - current_;
        current_ = now;
        time_point_ += diff;
        for (int i = 0; i < diff/TIME_UNIT; i++)
        {
            tick();
        }
    }
}