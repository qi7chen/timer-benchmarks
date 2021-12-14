// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include <stdint.h>
#include <functional>

enum class TimeoutState
{
    Init = 0,
    Cancelled = 1,
    Expired = 2,
};

enum class TimerSchedType
{
    TIMER_PRIORITY_QUEUE,
    TIMER_QUAD_HEAP,
    TIMER_RBTREE,
    TIMER_HASHED_WHEEL,
    TIMER_HH_WHEEL,
};

const int64_t TIME_UNIT = 10; // centisecond, i.e. 1/100 second

// expiry action
typedef std::function<void()> TimeoutAction;
