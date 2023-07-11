// Copyright (C) 2021 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HHWheelTimer.h"
#include "Clock.h"
#include "Logging.h"


struct timer_list
{
    hlist_node entry;
    int64_t expires = 0;
    int index = -1;         // bucket index
    int id = 0;             // unique id
    TimeoutAction action;   // timeout action

    void detach(bool clear_pending)
    {
        struct hlist_node *entry = &this->entry;

        // debug_deactivate(timer);

        __hlist_del(entry);
        if (clear_pending) {
            entry->pprev = NULL;
        }
        entry->next = NULL; // LIST_POISON2
    }

    bool is_pending()
    {
        return entry.pprev != nullptr;
    }
};


/*
 * Helper function to calculate the array index for a given expiry
 * time.
 */
static inline unsigned calc_index(int64_t expires, unsigned lvl)
{
    expires = (expires + LVL_GRAN(lvl)) >> LVL_SHIFT(lvl);
    return LVL_OFFS(lvl) + (expires & LVL_MASK);
}


static int calc_wheel_index(int64_t expires, int64_t clk)
{
    int64_t delta = expires - clk;
    int idx = 0;
    if (delta < 0) {
        idx = clk & LVL_MASK;
    }
    else 	if (delta < LVL_START(1)) {
        idx = calc_index(expires, 0);
    }
    else if (delta < LVL_START(2)) {
        idx = calc_index(expires, 1);
    }
    else if (delta < LVL_START(3)) {
        idx = calc_index(expires, 2);
    }
    else if (delta < LVL_START(4)) {
        idx = calc_index(expires, 3);
    }
    else if (delta < LVL_START(5)) {
        idx = calc_index(expires, 4);
    }
    else if (delta < LVL_START(6)) {
        idx = calc_index(expires, 5);
    }
    else if (delta < LVL_START(7)) {
        idx = calc_index(expires, 6);
    }
    else if (delta < LVL_START(8)) {
        idx = calc_index(expires, 7);
    }
    else {
        /*
         * Force expire obscene large timeouts to expire at the
         * capacity limit of the wheel.
         */
        if (delta >= WHEEL_TIMEOUT_CUTOFF)
            expires = clk + WHEEL_TIMEOUT_MAX;

        idx = calc_index(expires, LVL_DEPTH - 1);
    }
    return idx;
}


HHWheelTimer::HHWheelTimer()
{
    started_at_ = Clock::CurrentTimeMillis();
    purge();
}


HHWheelTimer::~HHWheelTimer()
{
    purge();
}

void HHWheelTimer::purge()
{
    size_ = 0;
    running_timer_ = nullptr;
    memset(&vectors_, 0, sizeof(vectors_));
    for (auto iter = ref_.begin(); iter != ref_.end(); ++iter)
    {
        delete iter->second;
        iter->second = nullptr;
    }
    ref_.clear();
}


int HHWheelTimer::Start(uint32_t ms, TimeoutAction action)
{
    int id = nextId();
    timer_list* node = new timer_list();
    node->id = id;
    node->action = action;
    node->expires = Clock::CurrentTimeMillis() + ms; // we assume time unit is ms
    internal_add_timer(node);
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
    return del_timer(iter->second);
}

int HHWheelTimer::Tick(int64_t ticks)
{
    // time_after_eq
    if (ticks < clk_) {
        return 0;
    }
    int fired = 0;
    hlist_head heads[LVL_DEPTH] = {};
    while (ticks >= clk_)
    {
        int levels = collect_expired(heads);
        clk_++;
        while (levels--)
        {
            fired += expire_timers(&heads[levels]);
        }
    }
    running_timer_ = nullptr;
    return fired;
}

int HHWheelTimer::collect_expired(hlist_head *heads)
{
    int64_t clk = clk_;
    int levels = 0;
    for (int i = 0; i < LVL_DEPTH; i++)
    {
        int idx = (clk & LVL_MASK) + 1 * LVL_SIZE;
        if (test_and_clear_bit(idx, pending_map_))
        {
            hlist_head *vec = &vectors_[idx];
            hlist_move_list(vec, heads++);
            levels++;
        }
        /* Is it time to look at the next level? */
        if (clk & LVL_CLK_MASK)
            break;
        /* Shift clock for the next level granularity */
        clk >>= LVL_CLK_SHIFT;
    }
    return levels;
}


int HHWheelTimer::expire_timers(hlist_head *head)
{
    int count = 0;
    while (!hlist_empty(head))
    {
        timer_list* timer = hlist_entry(head->first, timer_list, entry);
        running_timer_ = timer;
        timer->detach(true);
        timer->action();
    }
    return count;
}

bool HHWheelTimer::detach_if_pending(timer_list *timer, bool clear_pending)
{
    int idx = timer->index;
    if (timer->is_pending()) {
        return 0;
    }
    if (hlist_is_singular_node(&timer->entry, &vectors_[idx]))
    {
        clear_bit(idx, pending_map_);
    }
    timer->detach(clear_pending);
}


void HHWheelTimer::enqueue_timer(timer_list *timer, int idx)
{
    hlist_add_head(&timer->entry, &vectors_[idx]);
    set_bit(idx, pending_map_);
    timer->index = idx;
}

void HHWheelTimer::internal_add_timer(timer_list *timer)
{
    int idx = calc_wheel_index(timer->expires, clk_);
    enqueue_timer(timer, idx);
}

bool HHWheelTimer::del_timer(timer_list *timer)
{
    if (timer->is_pending())
    {
        return detach_if_pending(timer, true);
    }
    return false;
}

int HHWheelTimer::mod_timer(timer_list *timer, int64_t expires, bool pending_only)
{
    int idx = INT_MAX;
    int64_t clk = 0;
    /*
     * if the timer is re-modified to have the same timeout or ends up in the
     * same array bucket then just return:
     */
    if (timer->is_pending()) {
        /*
         * The downside of this optimization is that it can result in
         * larger granularity than you would get from adding a new
         * timer with this expiry.
         */
        if (timer->expires == expires) {
            return 1;
        }

        clk = clk_;
        idx = calc_wheel_index(expires, clk);

        /*
		 * Retrieve and compare the array index of the pending
		 * timer. If it matches set the expiry to the new value so a
		 * subsequent call will exit in the expires check above.
		 */
        if (idx == timer->index) {
            timer->expires = expires;
            return 1;
        }
    }

    bool ret = detach_if_pending(timer, false);
    if (!ret && pending_only) {
        return ret;
    }

    timer->expires = expires;

    /*
     * If 'idx' was calculated above and the base time did not advance
     * between calculating 'idx' and possibly switching the base, only
     * enqueue_timer() and trigger_dyntick_cpu() is required. Otherwise
     * we need to (re)calculate the wheel index via
     * internal_add_timer().
     */
    if (idx != INT_MAX && clk == clk_) {
        enqueue_timer(timer, idx);
    }
    else {
        internal_add_timer(timer);
    }
    return ret;
}
