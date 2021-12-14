// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"
#include <vector>
#include <unordered_map>

class HashedWheelBucket;
class HashedWheelTimeout;

// A simple hashed wheel timer inspired by [Netty HashedWheelTimer]
// see https://github.com/netty/netty/blob/4.1/common/src/main/java/io/netty/util/HashedWheelTimer.java
class HashedWheelTimer : public TimerBase
{
public:
    HashedWheelTimer();
    ~HashedWheelTimer();

    // start a timer after `ms` milliseconds
    int Start(uint32_t ms, TimeoutAction action) override;

    // stop a timer
    bool Stop(int timer_id) override;

    int Tick(int64_t now = 0) override;

    int Size() const override
    {
        return (int)ref_.size();
    }

private:
    friend class HashedWheelTimeout;
    friend class HashedWheelBucket;

    int tick();

    void purge();
    void delTimeout(HashedWheelTimeout*);

    HashedWheelTimeout* allocTimeout(int id, int64_t deadline, TimeoutAction action);
    void freeTimeout(HashedWheelTimeout*);

private:
    std::vector<HashedWheelBucket*> wheel_;
    std::unordered_map<int, HashedWheelTimeout*> ref_;
    
    int ticks_ = 0;
    int64_t started_at_ = 0;
    int64_t last_time_ = 0;
};
