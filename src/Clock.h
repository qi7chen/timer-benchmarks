// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include <stdint.h>
#include <string>


// Get current unix time
int64_t GetNowTime();

// Get current time in string format
std::string CurrentTimeString(int64_t timepoint);

// Get current tick count
uint64_t getNowTickCount();
