// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.


#include "QuadHeapTimer.h"
#include "Clock.h"

QuadHeapTimer::QuadHeapTimer()
{
}


QuadHeapTimer::~QuadHeapTimer()
{
    clear();
}

void QuadHeapTimer::clear()
{
    
}

int QuadHeapTimer::Start(uint32_t time_units, TimeoutAction action)
{
    return 0;
}

bool QuadHeapTimer::Stop(int timer_id)
{
    return false;
}

int QuadHeapTimer::Tick(int64_t now)
{
    return 0;
}
