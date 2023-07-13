// Copyright © 2021 ichenq@gmail.com All rights reserved.
// See accompanying files LICENSE

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>


int main(int argc, char* argv[])
{
    srand((int)time(NULL));

    testing::InitGoogleTest(&argc, argv);

    int r = RUN_ALL_TESTS();
    if (r == 1) {
        return 1;
    }

    printf("start run benchmarks\n");
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }
    benchmark::RunSpecifiedBenchmarks();                              
    benchmark::Shutdown();

    return 0;
}
