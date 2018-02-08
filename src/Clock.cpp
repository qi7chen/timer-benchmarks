// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "Clock.h"
#include <chrono>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "Logging.h"

int64_t GetNowTime()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

std::string CurrentTimeString(int64_t timepoint)
{
    time_t sec = timepoint / 1000;
    struct tm* pinfo = localtime(&sec); // make sure localtime has thread-safety
    char buffer[100] = {};
    int n = snprintf(buffer, 100, "%d-%02d-%02d %02d:%02d:%02d.%03d", 1900 + pinfo->tm_year, pinfo->tm_mon + 1,
        pinfo->tm_mday, pinfo->tm_hour, pinfo->tm_min, pinfo->tm_sec, int(timepoint % 1000));
    return std::string(buffer, n);
}


#ifdef _WIN32
static uint64_t getPerformanceFreqency()
{
    uint64_t freq;
    CHECK(QueryPerformanceFrequency((LARGE_INTEGER*)&freq));
    return freq;
};
#endif

uint64_t getNowTickCount()
{
#ifdef _WIN32
    static uint64_t freq = getPerformanceFreqency();
    uint64_t now;
    CHECK(QueryPerformanceCounter((LARGE_INTEGER*)&now));
    return (now * 1000000000UL) / freq;
#else
    timespec ts;
    CHECK(clock_gettime(CLOCK_REALTIME, &ts) == 0);
    return (ts.tv_sec * 1000000000UL) + ts.tv_nsec;
#endif
}
