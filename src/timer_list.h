#pragma once

#include <stdint.h>
#include "list_impl.h"


/*
 *	These inlines deal with timer wrapping correctly. You are
 *	strongly encouraged to use them
 *	1. Because people otherwise forget
 *	2. Because if the timer wrap changes in future you won't have to
 *	   alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
#define time_after(a,b)     ((int64_t)(b) - (int64_t)(a) < 0)
#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	((int64_t)(a) - (int64_t)(b) >= 0)
#define time_before_eq(a,b)	time_after_eq(b,a)

 /*
  * timer vector definitions
  */
#define TVN_BITS (6)
#define TVR_BITS (8)
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)
#define MAX_TVAL ((uint64_t)((1ULL << (TVR_BITS + 4*TVN_BITS)) - 1))


struct tvec {
    list_head vec[TVN_SIZE];
};

struct tvec_root {
    list_head vec[TVR_SIZE];
};

struct tvec_base;

struct timer_list {
    struct list_head entry;
    int64_t expires = 0;
    tvec_base* base = NULL;
    void (*function)(timer_list*) = NULL;
};

struct tvec_base {
    timer_list* running_timer = NULL;
    int64_t timer_clk = 0;
    struct tvec_root tv1;
    struct tvec tv2;
    struct tvec tv3;
    struct tvec tv4;
    struct tvec tv5;
};


// is a timer pending ?
inline int timer_pending(const struct timer_list* timer)
{
    return timer->entry.next != NULL;
}

void init_timers(struct tvec_base* base, int64_t clock);

/*
 * mod_timer_pending - modify a pending timer's timeout
 * @timer: the pending timer to be modified
 * @expires: new timeout in jiffies
 *
 * mod_timer_pending() is the same for pending timers as mod_timer(),
 * but will not re-activate and modify already deleted timers.
 *
 * It is useful for unserialized use of timers.
 */
int mod_timer_pending(struct timer_list* timer, int64_t expires);

/*
 * mod_timer - modify a timer's timeout
 * @timer: the timer to be modified
 * @expires: new timeout 
 *
 * mod_timer() is a more efficient way to update the expire field of an
 * active timer (if the timer is inactive it will be activated)
 *
 * mod_timer(timer, expires) is equivalent to:
 *
 *     del_timer(timer); timer->expires = expires; add_timer(timer);
 *
 *
 * The function returns whether it has modified a pending timer or not.
 * (ie. mod_timer() of an inactive timer returns 0, mod_timer() of an
 * active timer returns 1.)
 */
int mod_timer(struct timer_list* timer, int64_t expires);

/*
 * add_timer - start a timer
 * @timer: the timer to be added
 *
 * Timers with an ->expires field in the past will be executed in the next
 * timer tick.
 */
void add_timer(struct timer_list* timer);

/*
 * del_timer - deactive a timer.
 * @timer: the timer to be deactivated
 *
 * del_timer() deactivates a timer - this works on both active and inactive
 * timers.
 *
 * The function returns whether it has deactivated a pending timer or not.
 * (ie. del_timer() of an inactive timer returns 0, del_timer() of an
 * active timer returns 1.)
 */
int del_timer(struct timer_list* timer);

/**
 * run_timers - run all expired timers (if any).
 * @base: the timer vector to be processed.
 *
 * This function cascades all vectors and executes all expired timer
 * vectors.
 */
int run_timers(struct tvec_base* base, int64_t clock);
