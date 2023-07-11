// Copyright (C) 2021 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "Clock.h"
#include <chrono>
#include "Logging.h"
#include "CmdFlag.h"

using namespace std::chrono;

int64_t Clock::clock_offset_ = 0;

int64_t Clock::CurrentTimeMillis()
{
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
    return ms + clock_offset_;
}

void Clock::TimeReset()
{
    clock_offset_ = 0;
}

void Clock::TimeFly(int64_t ms)
{
    clock_offset_ += ms;
}

std::string Clock::CurrentTimeString(int64_t timepoint)
{
    if (timepoint == 0)
    {
        timepoint = CurrentTimeMillis();
    }
    time_t sec = timepoint / 1000;
    struct tm t = *localtime(&sec); // make sure localtime has thread-safety
    char buffer[100] = {};
    int n = snprintf(buffer, 100, "%d-%02d-%02d %02d:%02d:%02d.%03d", 1900 + t.tm_year, t.tm_mon + 1,
        t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, int(timepoint % 1000));
    return std::string(buffer, n);
}


int64_t Clock::GetNowTickCount()
{
    auto now = std::chrono::high_resolution_clock::now();
    return duration_cast<nanoseconds>(now.time_since_epoch()).count();
}

