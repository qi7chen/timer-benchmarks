// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "TimerQueueBase.h"

int TimerQueueBase::nextCounter()
{
    int next = counter_ + 1;
    for (;;)
    {
        next = next < 0 ? 0 : next;
        if (ref_.count(next) > 0)
        {
            next++;
            continue;
        }
        break;
    }
    counter_ = next;
    return next;
}
