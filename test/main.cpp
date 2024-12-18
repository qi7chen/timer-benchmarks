// Copyright © 2021 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include <thread>
#include "TimerBase.h"
#include "Clock.h"

int main(int argc, char* argv[])
{
    // srand((int)time(NULL));

    // testing::InitGoogleTest(&argc, argv);

    // int r = RUN_ALL_TESTS();
    // if (r == 1) {
    //     return 1;
    // }

    // printf("start run benchmarks\n");
    // benchmark::Initialize(&argc, argv);
    // if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    //     return 1;
    // }
    // benchmark::RunSpecifiedBenchmarks();                              
    // benchmark::Shutdown();

    int64_t deadline = Clock::CurrentTimeMillis();
    auto timer = CreateTimer(TimerSchedType::TIMER_HH_WHEEL);
    std::vector<int> times = {15,10,5,3,2,10,5,3,2,1,2,1,1,2000};
    auto delay = 0;
    std::cout << " start " << Clock::CurrentTimeMillis() << std::endl;
    uint32_t duration = 0;
    for (int i = 0; i < times.size(); i++) {

        duration += times[uint32_t(delay)%times.size()];
        std::cout << " duration " << duration << " cur ms: " << Clock::CurrentTimeMillis() << std::endl;
        int tid = timer->Start(duration, []() {

            std::cout << "client send 1111111        " << Clock::CurrentTimeMillis() << std::endl;


        });
        delay += 1;
    }
    for (int i = 0; i <= duration*30; i++) {
        timer->Update(Clock::CurrentTimeMillis());
        std::this_thread::sleep_for(std::chrono::nanoseconds(500 * 1000));
    }

    return 0;
}
