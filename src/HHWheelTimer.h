// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"
#include "HHUtil.h"
#include <unordered_map>

// Hashed and Hierarchical Timing Wheels
// see https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/kernel/time/timer.c?h=v5.10.116
//
// timer scheduler implemented by hashed & hierachical wheels
// complexity:
//      StartTimer   CancelTimer   PerTick
//       O(1)         O(1)          O(1)
//
class HHWheelTimer : public TimerBase
{
public:
    HHWheelTimer();
    ~HHWheelTimer();

    // start a timer after `ms` milliseconds
    int Start(uint32_t ms, TimeoutAction action) override;

    // cancel a timer
    bool Cancel(int timer_id) override;

    // we assume 1 tick per ms
    int Tick(int64_t ticks) override;

    int Size() const override
    {
        return (int)ref_.size();
    }

private:
    void purge();
    bool detach_if_pending(timer_list *timer, bool clear_pending);
    void enqueue_timer(timer_list *timer, int idx);
    void internal_add_timer(timer_list *timer);
    bool del_timer(timer_list *timer);
    int mod_timer(timer_list *timer, int64_t expires, bool pending_only);
    int collect_expired(hlist_head *heads);
    int expire_timers(hlist_head *heads);

private:
    int64_t started_at_ = 0;
    int size_ = 0;
    timer_list* running_timer_ = nullptr;
    int64_t clk_; //  current tick
    int64_t next_expiry = 0;
    uint64_t pending_map_[BITS_TO_LONGS(WHEEL_SIZE)];
    hlist_head	vectors_[WHEEL_SIZE];
    std::unordered_map<int, timer_list*> ref_;
};
