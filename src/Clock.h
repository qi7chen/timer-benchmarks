// Copyright © 2022 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#pragma once

#include <stdint.h>
#include <string>


class Clock
{
public:
    // Get current unix time in milliseconds
    static int64_t CurrentTimeMillis();

    // Get current time in string format
    static std::string CurrentTimeString(int64_t timepoint);

    // Get current tick count, in nanoseconds
    static int64_t GetNowTickCount();
    static void TimeFly(int64_t ms);
    static void TimeReset();

private:
    static int64_t clock_offset_; // offset time of logic clock
};
