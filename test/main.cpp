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
    RUN_ALL_TESTS();

#ifdef NDEBUG
    cout << "\nPATIENCE, BENCHMARKS IN PROGRESS." << endl;
    runBenchmarks();
    cout << "MEASUREMENTS DONE." << endl;
#endif 

    return 0;
}
