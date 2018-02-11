/*
 * Copyright 2014 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// @author Andrei Alexandrescu (andrei.alexandrescu@fb.com)

#include "Benchmark.h"
#include <cmath>
#include <ctime>
#include <cassert>
#include <cstring>
#include <limits>
#include <vector>
#include <tuple>
#include <memory>
#include <regex>
#include <algorithm>
#include <iostream>
#include "Logging.h"
#include "CmdLineFlags.h"


using namespace std;

DEFINE_bool(benchmark, true, "Run benchmarks.");

DEFINE_string(bm_regex, "",
    "Only benchmarks whose names match this regex will be run.");

DEFINE_int32(bm_min_usec, 100,
    "Minimum # of microseconds we'll accept for each benchmark.");

DEFINE_int32(bm_min_iters, 1,
    "Minimum # of iterations we'll try for each benchmark.");

DEFINE_int32(bm_max_secs, 1,
    "Maximum # of seconds we'll spend on each benchmark.");


BenchmarkSuspender::NanosecondsSpent BenchmarkSuspender::nsSpent;


namespace {

typedef function<detail::TimeIterPair(unsigned int)> BenchmarkFun;
typedef tuple<const char*, const char*, BenchmarkFun> BenchmarkItem;

// To avoid static initialization order fiasco
vector<BenchmarkItem>& getBenchmarks()
{
    static vector<BenchmarkItem> benchmarks;
    return benchmarks;
}

vector<detail::BenchmarkInitializer>& getBenchmarkInitializerList()
{
    static vector<detail::BenchmarkInitializer> initialier_list;
    return initialier_list;
}

} // anonymous namespace


// Add the global baseline, do nothing but measures the overheads.
BENCHMARK(globalBenchmarkBaseline, n)
{
#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("");
#endif
}

void detail::addBenchmarkInit(std::function<void(void)> initializer)
{
    auto& init_list = getBenchmarkInitializerList();
    init_list.emplace_back(initializer);
}

void detail::addBenchmarkImpl(const char* file,
                              const char* name,
                              BenchmarkFun fun)
{
    auto item = make_tuple(file, name, fun);
    auto& benchmarks = getBenchmarks();
    if (strcmp(name, "globalBenchmarkBaseline") == 0)
    {
        benchmarks.emplace(benchmarks.begin(), item);
    }
    else
    {
        benchmarks.emplace_back(item);
    }
}


/**
 * Given a bunch of benchmark samples, estimate the actual run time.
 */
inline double estimateTime(double * begin, double * end)
{
    assert(begin < end);

    // Current state of the art: get the minimum. After some
    // experimentation, it seems taking the minimum is the best.
    return *min_element(begin, end);
}


static double runBenchmarkGetNSPerIteration(const BenchmarkFun& fun, const double globalBaseline)
{
    // We choose a minimum (sic) of 100,000 nanoseconds
    static const auto minNanoseconds = FLAGS_bm_min_usec * 1000UL;

    // We do measurements in several epochs and take the minimum, to
    // account for jitter.
    static const unsigned int epochs = 1000;

    // We establish a total time budget as we don't want a measurement
    // to take too long. This will curtail the number of actual epochs.
    const uint64_t timeBudgetInNs = FLAGS_bm_max_secs * 1000000000;

    double epochResults[epochs] = { 0 };

    uint64_t global = Clock::GetNowTickCount();

    size_t actualEpochs = 0;
    for (; actualEpochs < epochs; ++actualEpochs)
    {
        for (auto n = FLAGS_bm_min_iters; n < (1UL << 30); n *= 2)
        {
            auto const nsecsAndIter = fun(n); // run `n` times of benchmark case
            if (nsecsAndIter.first < minNanoseconds)
            {
                continue;
            }
            // We got an accurate enough timing, done. But only save if
            // smaller than the current result.
            epochResults[actualEpochs] = max(0.0, double(nsecsAndIter.first) /
                nsecsAndIter.second - globalBaseline);
            // Done with the current epoch, we got a meaningful timing.
            break;
        }
        if (Clock::GetNowTickCount() - global >= timeBudgetInNs)
        {
            // No more time budget available.
            ++actualEpochs;
            break;
        }
    }
    // If the benchmark was basically drowned in baseline noise, it's
    // possible it became negative.
    return max(0.0, estimateTime(epochResults, epochResults + actualEpochs));
}

static void printBenchmarkResultsAsTable(
    const vector<tuple<const char*, const char*, double> >& data);


void runBenchmarks()
{
    auto& init_list = getBenchmarkInitializerList();
    for (auto& func : init_list)
    {
        func();
    }

    auto& benchmarks = getBenchmarks();
    CHECK(!benchmarks.empty());
    vector<tuple<const char*, const char*, double>> results;
    results.reserve(benchmarks.size() - 1);

    unique_ptr<regex> bmRegex;
    if (!FLAGS_bm_regex.empty())
    {
        bmRegex.reset(new regex(FLAGS_bm_regex));
    }

    // PLEASE KEEP QUIET. MEASUREMENTS IN PROGRESS.
    auto const globalBaseline = runBenchmarkGetNSPerIteration(
        get<2>(getBenchmarks().front()), 0);
    for (auto i = 1U; i < benchmarks.size(); i++)
    {
        double elapsed = 0.0;
        if (strcmp(get<1>(benchmarks[i]), "-") != 0) // skip separators
        {
            if (bmRegex && !regex_search(get<1>(benchmarks[i]), *bmRegex))
            {
                continue;
            }
            elapsed = runBenchmarkGetNSPerIteration(get<2>(benchmarks[i]),
                globalBaseline);
        }
        results.emplace_back(get<0>(benchmarks[i]), get<1>(benchmarks[i]), 
            elapsed);
    }

    printBenchmarkResultsAsTable(results);
}


struct ScaleInfo
{
    double boundary;
    const char* suffix;
};

static const ScaleInfo kTimeSuffixes[]
{
    { 365.25 * 24 * 3600, "years" },
    { 24 * 3600, "days" },
    { 3600, "hr" },
    { 60, "min" },
    { 1, "s" },
    { 1E-3, "ms" },
    { 1E-6, "us" },
    { 1E-9, "ns" },
    { 1E-12, "ps" },
    { 1E-15, "fs" },
    { 0, nullptr },
};

static const ScaleInfo kMetricSuffixes[]
{
    { 1E24, "Y" },  // yotta
    { 1E21, "Z" },  // zetta
    { 1E18, "X" },  // "exa" written with suffix 'X' so as to not create
    //   confusion with scientific notation
    { 1E15, "P" },  // peta
    { 1E12, "T" },  // terra
    { 1E9, "G" },   // giga
    { 1E6, "M" },   // mega
    { 1E3, "K" },   // kilo
    { 1, "" },
    { 1E-3, "m" },  // milli
    { 1E-6, "u" },  // micro
    { 1E-9, "n" },  // nano
    { 1E-12, "p" }, // pico
    { 1E-15, "f" }, // femto
    { 1E-18, "a" }, // atto
    { 1E-21, "z" }, // zepto
    { 1E-24, "y" }, // yocto
    { 0, nullptr },
};

static string humanReadable(double n, unsigned int decimals, const ScaleInfo* scales)
{
    char readableString[128];
    if (std::isinf(n) || std::isnan(n))
    {
        snprintf(readableString, 128, "%f", n);
        return readableString;
    }

    const double absValue = fabs(n);
    const ScaleInfo* scale = scales;
    while (absValue < scale[0].boundary && scale[1].suffix != nullptr)
    {
        ++scale;
    }

    const double scaledValue = n / scale->boundary;
    snprintf(readableString, 128, "%.*f%s", decimals, scaledValue, scale->suffix);
    return readableString;
}

inline string readableTime(double n, unsigned int decimals)
{
    return humanReadable(n, decimals, kTimeSuffixes);
}

inline string metricReadable(double n, unsigned int decimals)
{
    return humanReadable(n, decimals, kMetricSuffixes);
}

void printBenchmarkResultsAsTable(
    const vector<tuple<const char*, const char*, double> >& data)
{
    // Width available
    static const unsigned int columns = 76;

    // Compute the longest benchmark name
    size_t longestName = 0;
    auto& benchmarks = getBenchmarks();
    for (auto i = 1U; i < benchmarks.size(); i++)
    {
        longestName = max(longestName, strlen(get<1>(benchmarks[i])));
    }

    // Print a horizontal rule
    auto separator = [&](char pad)
    {
        puts(string(columns, pad).c_str());
    };

    // Print header for a file
    auto header = [&](const char* file)
    {
        separator('=');
        printf("%-*s relative  time/iter  iters/s\n",
            columns - 28, file);
        separator('=');
    };

    double baselineNsPerIter = numeric_limits<double>::max();
    const char* lastFile = "";

    for (auto& datum : data)
    {
        auto file = get<0>(datum);
        if (strcmp(file, lastFile))
        {
            // New file starting
            header(file);
            lastFile = file;
        }

        string s = get<1>(datum);
        if (s == "-")
        {
            separator('-');
            continue;
        }
        bool useBaseline /* = void */;
        if (s[0] == '%')
        {
            s.erase(0, 1);
            useBaseline = true;
        }
        else
        {
            baselineNsPerIter = get<2>(datum);
            useBaseline = false;
        }
        s.resize(columns - 29, ' ');
        auto nsPerIter = get<2>(datum);
        auto secPerIter = nsPerIter / 1E9;
        auto itersPerSec = 1 / secPerIter;
        auto secPerIterReadable = readableTime(secPerIter, 2);
        auto itersPerSecReadable = metricReadable(itersPerSec, 2);
        if (!useBaseline)
        {
            // Print without baseline
            printf("%*s           %9s  %7s\n",
                static_cast<int>(s.size()), s.c_str(),
                secPerIterReadable.c_str(),
                itersPerSecReadable.c_str());
        }
        else
        {
            // Print with baseline
            auto rel = baselineNsPerIter / nsPerIter * 100.0;
            auto relReadable = metricReadable(rel, 2);
            printf("%*s %7s%%  %9s  %7s\n",
                static_cast<int>(s.size()), s.c_str(),
                relReadable.c_str(),
                secPerIterReadable.c_str(),
                itersPerSecReadable.c_str());
        }
    }
    separator('=');
}
