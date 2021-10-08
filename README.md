# timerqueue-benchmark

分别使用最小堆、红黑树、时间轮实现定时器，再用随机数据对插入、删除、循环做性能测试。

关于定时器的简单介绍，可以参考[这篇文章](https://www.ibm.com/developerworks/cn/linux/l-cn-timers/index.html)


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
algo   | Add()    | Cancel() | Tick()   | implement
--------------------------------------------------------
最小堆 | O(log N) | O(N)     | O(1)     | src/PQTimer.h
红黑树 | O(log N) | O(N)     | O(log N) | src/TreeTimer.h
时间轮 | O(1)     | O(1)     | O(1)     | src/WheelTimer.h
```

最小堆和红黑树的Cancel均使用for遍历找到id做删除，所以都是O(N)复杂度。



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

* Windows和Linux两个平台有一些差异；
* 相比之下，最小堆PerTick()性能消耗最少；
* 最小堆的编码实现最简单，大多数编程语言都可以迅速实现，实现难度：最小堆 < 时间轮 < 红黑树；


## TO-DO

* 非STL版的红黑树实现；
* 时间轮的删除实现；
