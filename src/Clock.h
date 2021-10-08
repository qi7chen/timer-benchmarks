// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include <stdint.h>
#include <string>

/**
 * Call DoNotOptimize(var) to ensure that var will be computed even
 * post-optimization.  Use it for variables that are computed during
 * benchmarking but otherwise are useless. The compiler tends to do a
 * good job at eliminating unused variables, and this function fools it
 * into thinking var is in fact needed.
 */
#if defined(_MSC_VER) && !defined(__clang__)

#pragma optimize("", off)

inline void doNotOptimizeDependencySink(const void*) {}

#pragma optimize("", on)

template <class T>
void DoNotOptimize(const T& datum) {
    doNotOptimizeDependencySink(&datum);
}

#endif

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
