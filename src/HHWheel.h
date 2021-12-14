// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include <stdint.h>
#include "define.h"


// timer queue implemented with hashed hierarchical wheel.
//
// inspired by linux kernel, see links below
// https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/kernel/timer.c?h=v3.2.98
//
// We model timers as the number of ticks until the next
// due event. We allow 32-bits of space to track this
// due interval, and break that into 4 regions of 8 bits.
// Each region indexes into a bucket of 256 lists.

#define TVN_BITS        6      // time vector level shift bits
#define TVR_BITS        8      // timer vector shift bits

#define TVN_SIZE  (1 << TVN_BITS)
#define TVR_SIZE  (1 << TVR_BITS)

#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)

const int64_t MAX_TVAL = int64_t(TVN_SIZE) * int64_t(TVN_SIZE) * int64_t(TVN_SIZE) * int64_t(TVN_SIZE) * int64_t(TVR_SIZE);

struct WheelTimerBucket;

struct WheelTimerNode
{
    WheelTimerNode* next = nullptr;
    WheelTimerNode* prev = nullptr;
    WheelTimerBucket* bucket = nullptr;

    int id = -1;                // unique timer id
    int64_t deadline = -1;      // expired time in ms
    TimeoutAction action;       // expiry action

    void Expire();
    void Remove();
};

struct WheelTimerBucket
{
    WheelTimerNode* head = nullptr;
    WheelTimerNode* tail = nullptr;

    void AddNode(WheelTimerNode* node);
    WheelTimerNode* Remove(WheelTimerNode* node);
    WheelTimerNode* Splice();
    void Clear();
};
