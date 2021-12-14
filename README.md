# timerqueue-benchmark

as [Hashed and Hierarchical Timing Wheels](http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf) implies

a timer module has 3 component routines:

``` C++
// start a timer that will expire after `interval` unit of time
int Start(interval, expiry_action)

// use `tiemr_id` to locate a timer and stop it
Stop(timer_id)

// per-tick bookking routine
Tick()
```

use [min-heap](https://en.wikipedia.org/wiki/Heap_(data_structure)), quaternary heap or [4-ary heap](https://en.wikipedia.org/wiki/D-ary_heap), balanced binary search tree or [red-black tree](https://en.wikipedia.org/wiki/Red-black_tree), hashed timing wheel 
and Hierarchical timing wheel to model different time module 

分别使用最小堆、四叉堆、红黑树、时间轮、层级时间轮实现定时器，测试插入、删除、Tick操作的性能。


# 如何构建本项目

[Benchmark.h](src/Benchmark.h)取自[folly benchmark](https://github.com/facebook/folly/blob/master/folly/docs/Benchmark.md)

### 本项目依赖:

* [gtest](https://github.com/google/googletest)

### 构建工具

使用[CMake](https://cmake.org/download/)


### Linux或者[WSL](https://docs.microsoft.com/en-us/windows/wsl/install)构建

* `make build`

### Windows 构建

* `cmake -Bbuilds -DCMAKE_BUILD_TYPE=Release`
* `cmake --build builds --config Release`



# 性能测试

## 算法复杂度

复杂度比较：

```
algo   | Add()    | Cancel() | Tick()   | implemention
--------------------------------------------------------
binary heap         | O(log N) | O(N)     | O(1)     | src/PriorityQueueTimer.h
4-ary heap          | O(log N) | O(N)     | O(1)     | src/QuatHeapTimer.h
redblack tree       | O(log N) | O(N)     | O(log N) | src/RBTreeTimer.h
hashed timing wheel | O(1)     | O(1)     | O(1)     | src/HashedWheelTimer.h
hierarchical timing wheel | O(1)     | O(1)     | O(1)     | src/HHWheelTimer.h
```


Hardware: Ryzen 5 3600X 6-Core 3.79 GHz

OS: Windows 10 21H1



```
Compiler: GCC 9 (WSL)
============================================================================
test/BenchTimer.cpp                              relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  22.84ms    43.79
TreeTimerAdd                                      97.61%    23.40ms    42.74
WheelTimerAdd                                    418.14%     5.46ms   183.10
----------------------------------------------------------------------------
PQTimerDel                                                   8.81ms   113.55
TreeTimerDel                                     98.79m%      8.91s  112.17m
WheelTimerDel                                    247.61%     3.56ms   281.15
----------------------------------------------------------------------------
PQTimerTick                                                 17.86ms    56.01
TreeTimerTick                                    112.17%    15.92ms    62.82
WheelTimerTick                                    86.56%    20.63ms    48.48
============================================================================
============================================================================



```

```
Compiler: Visual C++ 2019 
============================================================================
test\benchtimer.cpp                             relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  11.86ms    84.32
TreeTimerAdd                                      79.11%    14.99ms    66.70
WheelTimerAdd                                    116.96%    10.14ms    98.63
----------------------------------------------------------------------------
PQTimerDel                                                   8.64ms   115.72
TreeTimerDel                                    106.92m%      8.08s  123.73m
WheelTimerDel                                    145.81%     5.93ms   168.73
----------------------------------------------------------------------------
PQTimerTick                                                  2.65ms   377.06
TreeTimerTick                                     66.44%     3.99ms   250.51
WheelTimerTick                                    46.78%     5.67ms   176.38
============================================================================
```


# 结论




## 参考

* [Hashed and Hierarchical Timing Wheels](https://paulcavallaro.com/blog/hashed-and-hierarchical-timing-wheels/)
* [Hashed and Hierarchical Timing Wheels](http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf)
* [Netty HashedWheelTimer](https://github.com/netty/netty/blob/4.1/common/src/main/java/io/netty/util/HashedWheelTimer.java)
* [Apache Kafka, Purgatory, and Hierarchical Timing Wheels](https://www.confluent.io/blog/apache-kafka-purgatory-hierarchical-timing-wheels/s)


