// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "Clock.h"
#include <chrono>
#ifdef _WIN32
#include <Windows.h>
#define snprintf sprintf_s
#endif
#include "Logging.h"

int64_t Clock::CurrentTimeMillis()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

int64_t Clock::CurrentTimeUnits()
{
    return CurrentTimeMillis() / TIME_UNIT;
}

std::string Clock::CurrentTimeString(int64_t timepoint)
{
    time_t sec = timepoint / 1000;
    struct tm* pinfo = localtime(&sec); // make sure localtime has thread-safety
    char buffer[100] = {};
    int n = snprintf(buffer, 100, "%d-%02d-%02d %02d:%02d:%02d.%03d", 1900 + pinfo->tm_year, pinfo->tm_mon + 1,
        pinfo->tm_mday, pinfo->tm_hour, pinfo->tm_min, pinfo->tm_sec, int(timepoint % 1000));
    return std::string(buffer, n);
}

#ifdef _WIN32
// get frequency of the performance counter
uint64_t GetPCFrequency()
{
    uint64_t freq = 0;
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&freq))
    {
        LOG(FATAL) << GetLastError();
    }
    return freq;
}
#endif

uint64_t Clock::GetNowTickCount()
{
#ifdef _WIN32
    static uint64_t freq = GetPCFrequency();
    uint64_t now = 0;
    if (!QueryPerformanceCounter((LARGE_INTEGER*)&now))
    {
        LOG(FATAL) << GetLastError();
    }
    return (now * 1000000000UL) / freq;
#else
    timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
    {
        LOG(FATAL) << errno;
    }
    return (ts.tv_sec * 1000000000UL) + ts.tv_nsec;
#endif
}
