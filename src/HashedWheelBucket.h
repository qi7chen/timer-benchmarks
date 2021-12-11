// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "TimerBase.h"
#include <vector>
#include <unordered_map>

enum class TimeoutState
{
    Init = 0,
    Cancelled = 1,
    Expired = 2,
};

class HashedWheelTimer;
class HashedWheelBucket;

class HashedWheelTimeout
{
public:
    HashedWheelTimeout(HashedWheelTimer* timer, int id, int64_t deadline, TimeoutAction aciton)
        : timer(timer), id(id), deadline(deadline), action(aciton)
    {
    }

    HashedWheelTimeout(const HashedWheelTimeout&) = delete;
    HashedWheelTimeout& operator=(const HashedWheelTimeout&) = delete;

    bool IsExpired() 
    {
        return state == TimeoutState::Init;
    }

    bool IsCanceled() 
    {
        return state == TimeoutState::Cancelled;
    }

    bool Cancel();
    bool Expire();
    void Remove();

    bool operator < (const HashedWheelTimeout& b) const
    {
        return deadline < b.deadline;
    }

public:
    HashedWheelTimeout* next = nullptr;
    HashedWheelTimeout* prev = nullptr;

    HashedWheelBucket* bucket = nullptr;
    HashedWheelTimer* timer = nullptr;

    TimeoutState state = TimeoutState::Init;  // cancelled or expired
    int32_t remaining_rounds = 0;
    int64_t deadline = 0;
    int id = 0;
    TimeoutAction action = nullptr;
};

// Bucket that stores HashedWheelTimeouts.
class HashedWheelBucket
{
public:
    HashedWheelBucket();
    ~HashedWheelBucket();

    HashedWheelBucket(const HashedWheelBucket&) = delete;
    HashedWheelBucket& operator=(const HashedWheelBucket&) = delete;

    void AddTimeout(HashedWheelTimeout* timeout);
    void ExpireTimeouts(int64_t deadline, std::vector< HashedWheelTimeout*>& expired);
    HashedWheelTimeout* Remove(HashedWheelTimeout* timeout);
    void ClearTimeouts(std::unordered_map<int, HashedWheelTimeout*>& set);

    void purge();

private:
    HashedWheelTimeout* pollTimeout();

    HashedWheelTimeout* head = nullptr;
    HashedWheelTimeout* tail = nullptr;
};