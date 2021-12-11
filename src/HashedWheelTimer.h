// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"
#include <vector>
#include <queue>
#include <unordered_map>

class HashedWheelBucket;
class HashedWheelTimeout;

// A simple hashed wheel timer
// [Netty HashedWheelTimer](https://github.com/netty/netty/blob/4.1/common/src/main/java/io/netty/util/HashedWheelTimer.java)
class HashedWheelTimer : public TimerBase
{
public:


public:
    HashedWheelTimer();
    ~HashedWheelTimer();

    int Start(uint32_t time_units, TimeoutAction action) override;

    bool Stop(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override
    {
        return size_;
    }

private:

    friend class HashedWheelTimeout;
    friend class HashedWheelBucket;

    void processCancelledTasks();
    void transferTimeoutToBuckets();

    void purge();

    void decrementPending()
    {
        size_--;
    }

private:

    std::vector<HashedWheelBucket*> wheel_;
    std::queue<HashedWheelTimeout*> timeouts_;
    std::queue<HashedWheelTimeout*> cancelled_timeouts_;
    std::unordered_map<int, HashedWheelTimeout*> ref_;
    int size_ = 0;
    int ticks_ = 0;
    int64_t started_at_ = 0;
};
