#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "Benchmark.h"

using std::cout;
using std::endl;


int main(int argc, char* argv[])
{
    srand((int)time(NULL));
    cout << "\nPATIENCE, BENCHMARKS IN PROGRESS." << endl;
    runBenchmarks();
    cout << "MEASUREMENTS DONE." << endl;
    return 0;
}
