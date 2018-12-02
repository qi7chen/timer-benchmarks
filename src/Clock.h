// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include <stdint.h>
#include <string>

enum
{
    TIME_UNIT = int64_t(10),       // centisecond, i.e. 1/100 second
};

class Clock
{
public:
    // Get current unix time in milliseconds
    static int64_t CurrentTimeMillis();

    // current time units in centiseconds, 1/100 seconds
    static int64_t CurrentTimeUnits();

    // Get current time in string format
    static std::string CurrentTimeString(int64_t timepoint);

    // Get current tick count, in nanoseconds
    static uint64_t GetNowTickCount();
};
