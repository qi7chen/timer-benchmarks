// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "WheelTimer.h"
#include "Clock.h"
#include <assert.h>
#include "Logging.h"


WheelTimer::WheelTimer()
    : current_(Clock::CurrentTimeUnits())
{
    ref_.rehash(64);       
}


WheelTimer::~WheelTimer()
{
    clearAll();
}

void WheelTimer::clearList(TimerList& list)
{
    for (auto ptr : list)
    {
        delete ptr;
    }
    list.clear();
}

void WheelTimer::clearAll()
{
    for (int i = 0; i < TVR_SIZE; i++)
    {
        clearList(near_[i]);
    }
    for (int i = 0; i < WHEEL_BUCKETS; i++)
    {
        for (int j = 0; j < TVN_SIZE; j++)
        {
            clearList(buckets_[i][j]);
        }
    }
    ref_.clear();
}

WheelTimer::TimerNode* WheelTimer::allocNode()
{
    return new TimerNode;
}


void WheelTimer::freeNode(TimerNode* node)
{
    delete node;
}

void WheelTimer::addTimerNode(TimerNode* node)
{
    int64_t expires = node->expire;
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
    list->push_back(node);
}

int WheelTimer::Schedule(uint32_t time_units, TimerCallback cb)
{
    TimerNode* node = allocNode();
    node->canceled = false;
    node->cb = cb;
    node->expire = jiffies_ + time_units;
    node->id = nextCounter();
    addTimerNode(node);
    ref_[node->id] = node;
    size_++;
    return node->id;
}

// Do lazy cancellation, so we can effectively use vector as container of timer nodes
bool WheelTimer::Cancel(int id)
{
    TimerNode* node = ref_[id];
    if (node != nullptr)
    {
        node->canceled = true;
        size_--;
        return true;
    }
    return false;
}

// cascade all the timers at bucket of index up one level
bool WheelTimer::cascade(int bucket, int index)
{
    // swap list
    TimerList list;
    buckets_[bucket][index].swap(list);

    for (auto node : list)
    {
        if (node->id > 0)
        {
            addTimerNode(node);
        }
    }
    return index == 0;
}

#define INDEX(N) ((jiffies_ >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

// cascades all vectors and executes all expired timer
int WheelTimer::tick()
{
    int fired = execute();
    int index = jiffies_ & TVR_MASK;
    if (index == 0) // cascade timers
    {
        if (cascade(0, INDEX(0)) &&
            cascade(1, INDEX(1)) &&
            cascade(2, INDEX(2)))
            cascade(3, INDEX(3));
    }
#undef INDEX
    
    jiffies_++;
    fired += execute();
    return fired;
}

int WheelTimer::execute()
{
    int fired = 0;
    int index = jiffies_ & TVR_MASK;
    TimerList expired;
    near_[index].swap(expired); // swap list
    for (auto node : expired)
    {
        if (!node->canceled && node->cb)
        {
            //printf("wheel node %d triggered at %lld of jiffies %lld\n", node->id, current_, jiffies_);
            node->cb();
            size_--;
            fired++;
        }

        ref_.erase(node->id);
        freeNode(node);
    }
    return fired;
}

int WheelTimer::Update(int64_t now)
{
    if (now == 0)
    {
        now = Clock::CurrentTimeUnits();
    }
    if (now < lastTs_)
    {
        assert(false && "time go backwards");
        lastTs_ = now;
        return -1;
    }
    else if (now > lastTs_)
    {
        int ticks = (int)(now - lastTs_);
        lastTs_ = now;
        int fired = 0;
        for (int i = 0; i < ticks; i++)
        {
            //printf("tick of jiffies %lld at %lld, %d/%d\n", jiffies_, current_, i, ticks);
            fired += tick();
        }
        return fired;
    }
    return 0;
}
