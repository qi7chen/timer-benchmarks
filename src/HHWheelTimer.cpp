// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HHWheelTimer.h"
#include "Clock.h"
#include "Logging.h"

HHWheelTimer::HHWheelTimer()
{
    started_at_ = Clock::CurrentTimeMillis();
    last_time_ = started_at_;
}


HHWheelTimer::~HHWheelTimer()
{
    purge();
}

void HHWheelTimer::purge()
{
    for (int i = 0; i < TVR_SIZE; i++)
    {
        purgeBucket(&near_[i]);
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < TVN_SIZE; j++)
        {
            purgeBucket(&buckets_[i][j]);
        }
    }
}

void HHWheelTimer::purgeBucket(WheelTimerBucket* bucket)
{
    WheelTimerNode* node = bucket->head;
    while (node != nullptr)
    {
        WheelTimerNode* next = node->next;
        deltimer(node);
        node = next;
    }
    bucket->head = nullptr;
    bucket->tail = nullptr;
}

int HHWheelTimer::Start(uint32_t ms, TimeoutAction action)
{
    int id = nextId();
    WheelTimerNode* node = new WheelTimerNode();
    node->id = id;
    node->action = action;
    node->deadline = Clock::CurrentTimeMillis() + ms;
    AddNode(node);
    ref_[id] = node;
    size_++;
    return id;
}

bool HHWheelTimer::Cancel(int timer_id)
{
    auto iter = ref_.find(timer_id);
    if (iter == ref_.end()) {
        return false;
    }
    deltimer(iter->second);
    return true;
}

void HHWheelTimer::deltimer(WheelTimerNode* node)
{
    size_--;
    node->Remove();
    ref_.erase(node->id);
    delete node;
}


int HHWheelTimer::Tick(int64_t now)
{
    if (Size() == 0)
    {
        return 0;
    }
    int64_t elapsed = now - last_time_;
    int64_t ticks = elapsed / TIME_UNIT;
    if (ticks <= 0) 
    {
        return 0;
    }
    last_time_ = now;
    int fired = 0;
    for (int64_t i = 0; i < ticks; i++)
    {
        fired += tick();
    }
    return fired;
}


bool HHWheelTimer::AddNode(WheelTimerNode* node)
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

void HHWheelTimer::Remove(WheelTimerNode* node)
{
    CHECK(node != nullptr);
    CHECK(node->bucket != NULL);
    node->bucket->Remove(node);
}

bool HHWheelTimer::Cascade(int level, int index)
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

int HHWheelTimer::ExpireNear()
{
    int count = 0;
    int idx = currtick_ & TVR_MASK;
    WheelTimerBucket* bucket = &near_[idx];
    WheelTimerNode* node = bucket->Splice();
    while (node != nullptr)
    {
        WheelTimerNode* next = node->next;
        node->Expire();
        deltimer(node);
        count++;
        node = next;
    }
    return count;
}

void HHWheelTimer::ShiftLevel()
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
int HHWheelTimer::tick()
{
    int count = 0;
    count += ExpireNear(); // try to dispatch timeout 0 (rare condition)
    currtick_++;
    ShiftLevel();
    count += ExpireNear();
    return count;
}

