// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "TimerQueueBase.h"

TimerQueueBase::TimerQueueBase()
{
}

TimerQueueBase::~TimerQueueBase()
{
}

// we assume you won't have too much timer in the same time
int TimerQueueBase::nextCounter()
{
    return ++counter_;
}
