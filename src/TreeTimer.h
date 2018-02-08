// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "ITimer.h"

class TreeTimer : public ITimerQueue
{
public:
    TreeTimer();
    ~TreeTimer();

    int AddTimer(int ms, TimerCallback cb) override;

    void CancelTimer(int id) override;

    void Tick(int64_t now) override;
};

