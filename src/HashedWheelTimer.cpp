// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "HashedWheelTimer.h"
#include "Clock.h"
#include <assert.h>
#include "Logging.h"


HashedWheelTimer::HashedWheelTimer()
{      
}


HashedWheelTimer::~HashedWheelTimer()
{
}


int HashedWheelTimer::Start(uint32_t time_units, TimeoutAction action)
{
    return 0;
}

bool HashedWheelTimer::Stop(int timer_id)
{
    return false;
}

int HashedWheelTimer::Tick(int64_t now)
{
    return 0;
}
