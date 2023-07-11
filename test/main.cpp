// Copyright (C) 2022 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <gtest/gtest.h>
#include "Benchmark.h"

using std::cout;
using std::endl;


int main(int argc, char* argv[])
{
    srand((int)time(NULL));

    testing::InitGoogleTest(&argc, argv);

    int r = RUN_ALL_TESTS();

    // only run benchmark in release mode
#if defined(NDEBUG)
    runBenchmarks();
#endif

    return r;
}
