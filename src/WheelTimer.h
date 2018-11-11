// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerQueueBase.h"
#include <vector>
#include <unordered_map>

// timer queue implemented with hashed hierarchical wheel.
//
// inspired by linux kernel, see links below
// https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git/tree/kernel/timer.c?h=v3.2.98
//
// We model timers as the number of ticks until the next
// due event. We allow 32-bits of space to track this
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

    MAX_TVAL = ((uint64_t)((1ULL << (TVR_BITS + 4 * TVN_BITS)) - 1)),

    TIME_UNIT = int64_t(10),       // centisecond, i.e. 1/100 second
};

class WheelTimer : public TimerQueueBase
{
public:
    struct TimerNode
    {
        bool canceled = false;  // do lazy cancellation
        int id = -1;
        int64_t expire = -1;     // jiffies
        TimerCallback cb;
    };

    typedef std::vector<TimerNode*> TimerList;

    const int FREE_LIST_CAPACITY = 1024;

public:
    WheelTimer();
    ~WheelTimer();

    int RunAfter(uint32_t milliseconds, TimerCallback cb) override;

    bool Cancel(int id) override;

    int Update(int64_t now = 0) override;

    int Size() const override 
    { 
        return size_; 
    }

private:
    int tick();
    void addTimerNode(TimerNode* node);
    int execute();
    bool cascade(int bucket, int index);
    void clearList(TimerList& list);
    void clearAll();
    TimerNode* allocNode();
    void freeNode(TimerNode*);

private:
    int size_ = 0;
    int64_t current_ = 0;
    int64_t jiffies_ = 0;
    TimerList near_[TVR_SIZE];
    TimerList buckets_[WHEEL_BUCKETS][TVN_SIZE];
    std::unordered_map<int, TimerNode*> ref_;       // make O(1) searching
    std::vector<TimerNode*>   free_list_;
};
