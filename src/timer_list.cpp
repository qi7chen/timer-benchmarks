// Distributed under GPLv3 license, see accompanying files LICENSE

#include "timer_list.h"
#include <assert.h>

void init_timers(struct tvec_base* base, int64_t clock)
{
    for (int j = 0; j < TVN_SIZE; j++) {
        INIT_LIST_HEAD(base->tv5.vec + j);
        INIT_LIST_HEAD(base->tv4.vec + j);
        INIT_LIST_HEAD(base->tv3.vec + j);
        INIT_LIST_HEAD(base->tv2.vec + j);
    }
    for (int j = 0; j < TVR_SIZE; j++)
        INIT_LIST_HEAD(base->tv1.vec + j);

    base->timer_clk = clock;
}

static inline void timer_set_base(struct timer_list* timer, struct tvec_base* new_base)
{
    timer->base = new_base;
}

static void __internal_add_timer(struct tvec_base* base, struct timer_list* timer)
{
    int64_t expires = timer->expires;
    int64_t idx = expires - base->timer_clk;
    struct list_head* vec;

    if (idx < TVR_SIZE) {
        int i = expires & TVR_MASK;
        vec = base->tv1.vec + i;
    }
    else if (idx < 1 << (TVR_BITS + TVN_BITS)) {
        int i = (expires >> TVR_BITS) & TVN_MASK;
        vec = base->tv2.vec + i;
    }
    else if (idx < 1 << (TVR_BITS + 2 * TVN_BITS)) {
        int i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
        vec = base->tv3.vec + i;
    }
    else if (idx < 1 << (TVR_BITS + 3 * TVN_BITS)) {
        int i = (expires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK;
        vec = base->tv4.vec + i;
    }
    else if (idx < 0) {
        /*
         * Can happen if you add a timer with expires == jiffies,
         * or you set a timer to go off in the past
         */
        vec = base->tv1.vec + (base->timer_clk & TVR_MASK);
    }
    else {
        int i;
        /* If the timeout is larger than MAX_TVAL (on 64-bit
         * architectures or with CONFIG_BASE_SMALL=1) then we
         * use the maximum timeout.
         */
        if (idx > MAX_TVAL) {
            idx = MAX_TVAL;
            expires = idx + base->timer_clk;
        }
        i = (expires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK;
        vec = base->tv5.vec + i;
    }
    /*
     * Timers are FIFO:
     */
    list_add_tail(&timer->entry, vec);
}

static void internal_add_timer(struct tvec_base* base, struct timer_list* timer)
{
    __internal_add_timer(base, timer);
    // Update base->active_timers and base->next_timer
}

static inline void detach_timer(struct timer_list* timer, bool clear_pending)
{
    struct list_head* entry = &timer->entry;

    __list_del(entry->prev, entry->next);
    if (clear_pending) {
        entry->next = NULL;
    }
    entry->prev = reinterpret_cast<list_head*>(LIST_POISON2);
}

static inline void detach_expired_timer(struct timer_list* timer, struct tvec_base* base)
{
    detach_timer(timer, true);
}

static int detach_if_pending(struct timer_list* timer, struct tvec_base* base, bool clear_pending)
{
    if (!timer_pending(timer)) {
        return 0;
    }

    detach_timer(timer, clear_pending);
    return 1;
}

static inline int __mod_timer(struct timer_list* timer, int64_t expires, bool pending_only)
{
    assert(!timer->function);
    int ret = detach_if_pending(timer, timer->base, false);
    if (!ret && pending_only) {
        return ret;
    }
    timer->expires = expires;
    internal_add_timer(timer->base, timer);
    return ret;
}

static void migrate_timer_list(struct tvec_base* new_base, struct list_head* head)
{
    struct timer_list* timer;

    while (!list_empty(head)) {
        timer = list_first_entry(head, struct timer_list, entry);
        /* We ignore the accounting on the dying cpu */
        detach_timer(timer, false);
        timer_set_base(timer, new_base);
        internal_add_timer(new_base, timer);
    }
}

int mod_timer_pending(struct timer_list* timer, int64_t expires)
{
    return __mod_timer(timer, expires, true);
}

int mod_timer(struct timer_list* timer, int64_t expires)
{
    if (timer_pending(timer) && timer->expires == expires) {
        return 1;
    }
    return __mod_timer(timer, expires, false);
}

void add_timer(struct timer_list* timer)
{
    assert(!timer_pending(timer));
    mod_timer(timer, timer->expires);
}

int del_timer(struct timer_list* timer)
{
    int ret = 0;
    if (timer_pending(timer)) {
        ret = detach_if_pending(timer, timer->base, true);
    }
    return ret;
}

static int cascade(struct tvec_base* base, struct tvec* tv, int index)
{
    /* cascade all the timers from tv up one level */
    struct list_head tv_list;

    list_replace_init(tv->vec + index, &tv_list);

    /*
     * We are removing _all_ timers from the list, so we
     * don't have to detach them individually.
     */
    timer_list* timer = list_entry(tv_list.next, timer_list, entry);
    timer_list* tmp = list_entry(timer->entry.next, timer_list, entry);

    while (&timer->entry != &tv_list)
    {
        timer = tmp;
        tmp = list_entry(tmp->entry.next, timer_list, entry);
        // BUG_ON(tbase_get_base(timer->base) != base);
        /* No accounting, while moving them */
        __internal_add_timer(base, timer);
    }

    return index;
}

inline void call_timer_fn(struct timer_list* timer, void (*fn)(timer_list*))
{
    assert(fn);
    //trace_timer_expire_entry(timer);
    fn(timer);
    //trace_timer_expire_exit(timer);
}

#define INDEX(N) ((base->timer_clk >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

int run_timers(struct tvec_base* base, int64_t clock)
{
    int n = 0;
    while (time_after_eq(clock, base->timer_clk)) {
        struct list_head work_list;
        struct list_head* head = &work_list;
        int index = base->timer_clk & TVR_MASK;
        
         // Cascade timers:
        if (!index &&
            (!cascade(base, &base->tv2, INDEX(0))) &&
            (!cascade(base, &base->tv3, INDEX(1))) &&
            !cascade(base, &base->tv4, INDEX(2))) {
            cascade(base, &base->tv5, INDEX(3));
        }
        ++base->timer_clk;
        list_replace_init(base->tv1.vec + index, &work_list);

        while (!list_empty(head)) {
            struct timer_list* timer = list_first_entry(head, timer_list, entry);
            auto fn = timer->function;
            base->running_timer = timer;
            detach_expired_timer(timer, base);
            call_timer_fn(timer, fn);
            n++;
        }
    }
    base->running_timer = NULL;
    return n;
}

