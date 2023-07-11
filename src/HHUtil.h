// Copyright (C) 2022 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#pragma once

#include <stdint.h>


// timer queue implemented with hashed hierarchical wheel.
//
// inspired by linux kernel, see links below
// https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/kernel/time/timer.c?h=v5.10.116

/* Level depth */
# define LVL_DEPTH	9

 /* Size of each clock level */
#define LVL_BITS	6
#define LVL_SIZE	(1UL << LVL_BITS)
#define LVL_MASK	(LVL_SIZE - 1)
#define LVL_OFFS(n)	((n) * LVL_SIZE)

/*
 * The resulting wheel size. If NOHZ is configured we allocate two
 * wheels so we have a separate storage for the deferrable timers.
 */
#define WHEEL_SIZE	(LVL_SIZE * LVL_DEPTH)

 /* Clock divisor for the next level */
#define LVL_CLK_SHIFT	3
#define LVL_CLK_DIV		(1ULL << LVL_CLK_SHIFT)
#define LVL_CLK_MASK	(LVL_CLK_DIV - 1)
#define LVL_SHIFT(n)	((n) * LVL_CLK_SHIFT)
#define LVL_GRAN(n)		(1ULL << LVL_SHIFT(n))

/*
 * The time start value for each level to select the bucket at enqueue
 * time.
 */
#define LVL_START(n)	((LVL_SIZE - 1) << (((n) - 1) * LVL_CLK_SHIFT))

 /* The cutoff (max. capacity of the wheel) */
#define WHEEL_TIMEOUT_CUTOFF	(LVL_START(LVL_DEPTH))
#define WHEEL_TIMEOUT_MAX		(WHEEL_TIMEOUT_CUTOFF - LVL_GRAN(LVL_DEPTH - 1))


#define __WORDSIZE (sizeof(uint64_t) * 8)

#ifndef BITS_PER_LONG
# define BITS_PER_LONG __WORDSIZE
#endif

#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))

#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, 8 * sizeof(uint64_t))

template <int N>
inline bool test_bit(int i, const uint64_t(&bitmap)[N]) {
    if (i >= 0 && i < N*BITS_PER_LONG) {
        return (bitmap[i / BITS_PER_LONG] & (1ULL << (i% BITS_PER_LONG))) != 0;
    }
    return false;
}

template <int N>
inline void set_bit(int i, uint64_t(&bitmap)[N]) {
    uint64_t v = 1ULL << (1 % BITS_PER_LONG);
    bitmap[i / BITS_PER_LONG] |= v;
}

template <int N>
inline void clear_bit(int i, uint64_t(&bitmap)[N]) {
    uint64_t v = 1ULL << (1 % BITS_PER_LONG);
    bitmap[i / BITS_PER_LONG] &= ~v;
}

template <int N>
inline bool test_and_clear_bit(int i, uint64_t(&bitmap)[N]) {
    bool v = test_bit(i, bitmap);
    clear_bit(i, bitmap);
    return v;
}

#ifndef container_of
#define container_of(ptr, type, member) \
    (type *)((char *)(ptr) - (char *) &((type *)0)->member)
#endif

#define hlist_entry(ptr, type, member) container_of(ptr,type,member)

struct list_head {
    struct list_head *next, *prev;
};

struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

static inline int hlist_empty(const struct hlist_head *h)
{
    return !h->first;
}

static inline void __hlist_del(struct hlist_node *n)
{
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;

    *pprev = next;
    if (next) {
        next->pprev = pprev;
    }
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
    struct hlist_node *first = h->first;
    n->next = first;
    if (first) {
        first->pprev = &n->next;
    }
    h->first = n;
    n->pprev = &h->first;
}

/*
 * Check whether the node is the only node of the head without
 * accessing head:
 */
static inline bool hlist_is_singular_node(struct hlist_node *n, struct hlist_head *h)
{
    return !n->next && n->pprev == &h->first;
}

/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
static inline void hlist_move_list(struct hlist_head *old, struct hlist_head *newi)
{
    newi->first = old->first;
    if (newi->first)
    {
        newi->first->pprev = &newi->first;
    }
    old->first = NULL;
}


struct timer_list;
