// Copyright (C) 2018 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "TimerBase.h"
#include "PriorityQueueTimer.h"
#include "QuadHeapTimer.h"
#include "RBTreeTimer.h"
#include "HashedWheelTimer.h"
#include "HHWheelTimer.h"

TimerBase::TimerBase()
{
}

TimerBase::~TimerBase()
{
}

int TimerBase::nextId()
{
    return ++next_id_; // no duplicate checking here
}


TimerBase* CreateTimer(TimerSchedType sched_type)
{
    switch (sched_type)
    {
    case TimerSchedType::TIMER_PRIORITY_QUEUE:
        return new PriorityQueueTimer();
    case TimerSchedType::TIMER_QUAD_HEAP:
        return new QuadHeapTimer();
    case TimerSchedType::TIMER_RBTREE:
        return new RBTreeTimer();
    case TimerSchedType::TIMER_HASHED_WHEEL:
        return new HashedWheelTimer();
    case TimerSchedType::TIMER_HH_WHEEL:
        return new HHWheelTimer();
    default:
        return nullptr;
    }
}