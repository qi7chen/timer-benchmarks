// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "ITimer.h"
#include <unordered_map>

// timer queue implemented with hashed hierarchical wheel.
//
// We model timers as the number of ticks until the next
// due event.We allow 32 - bits of space to track this
// due interval, and break that into 4 regions of 8 bits.
// Each region indexes into a bucket of 256 lists.
//
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(1)       O(1)          O(1)
//

enum
{
    WHEEL_BUCKETS = 4,
    TVN_BITS = 6,                   // time vector level shift bits
    TVR_BITS = 8,                   // timer vector shift bits
    TVN_SIZE = (1 << TVN_BITS),     // wheel slots of level vector
    TVR_SIZE = (1 << TVR_BITS),     // wheel slots of vector
    TVN_MASK = (TVN_SIZE - 1),      //
    TVR_MASK = (TVR_SIZE - 1),      //
};

class WheelTimer : public ITimerQueue
{
public:
    struct TimerNode
    {
        // do lazy cancellation, so single linked list is enough
        TimerNode* next = nullptr;
        bool canceld = false;
        int id = -1;
        int64_t expires = 0;
        TimerCallback cb;
    };

    struct TimerList
    {
        TimerNode head;
        TimerNode* tail = nullptr;

        TimerList()
        {
            tail = &head;
        }

        void reset()
        {
            tail = &head;
        }
    };

public:
    WheelTimer();
    ~WheelTimer();

    int AddTimer(uint32_t time, TimerCallback cb) override;

    void CancelTimer(int id) override;

    void Update(int64_t now) override;

private:
    void tick();
    void addTimerNode(TimerNode* node);
    void cascadeTimer(int bucket, int index);
    void clearAll();

private:
    int64_t current_ = 0;
    int64_t time_point_ = 0;
    int64_t jiffies_ = 0;
    int counter_ = 0;
    TimerList near_[TVN_SIZE];
    TimerList buckets_[WHEEL_BUCKETS][TVN_SIZE];
    std::unordered_map<int, TimerNode*> ref_;
};

