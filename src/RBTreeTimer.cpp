// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "RBTreeTimer.h"
#include "Clock.h"


RBTreeTimer::RBTreeTimer()
{
}


RBTreeTimer::~RBTreeTimer()
{
    clear();
}

void RBTreeTimer::clear()
{
    
}

int RBTreeTimer::Start(uint32_t time_units, TimeoutAction action)
{
    return 0;
}

bool RBTreeTimer::Stop(int timer_id)
{
    return false;
}

int RBTreeTimer::Tick(int64_t now)
{
    return 0;
}
