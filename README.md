# timerqueue-benchmark

分别使用最小堆、红黑树、时间轮实现定时器，再对插入、删除、循环做性能测试。

关于定时器的简单介绍，可以参考[这篇文章](https://www.ibm.com/developerworks/cn/linux/l-cn-timers/index.html)


# 如何构建本项目

* 下载[premake](https://premake.github.io/download.html#v5)
* 使用premake生成Visual Studio解决方案或者makefile，如`premake5 vs2017`


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



```
E5 2.3GHz CentOS 7
============================================================================
test/BenchTimer.cpp                              relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  54.28ms    18.42
TreeTimerAdd                                      72.07%    75.32ms    13.28
WheelTimerAdd                                    212.18%    25.58ms    39.09
----------------------------------------------------------------------------
PQTimerDel                                                  34.70ms    28.82
TreeTimerDel                                     667.01%     5.20ms   192.20
WheelTimerDel                                    383.55%     9.05ms   110.52
----------------------------------------------------------------------------
PQTimerTick                                                 41.90ms    23.87
TreeTimerTick                                     92.85%    45.12ms    22.16
WheelTimerTick                                    83.76%    50.02ms    19.99
============================================================================

```

```
i3 1.9GHz Windows 7
============================================================================
test\BenchTimer.cpp                              relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  32.89ms    30.41
TreeTimerAdd                                      63.83%    51.52ms    19.41
WheelTimerAdd                                     42.58%    77.24ms    12.95
----------------------------------------------------------------------------
PQTimerDel                                                 103.45ms     9.67
TreeTimerDel                                      1.04K%     9.92ms   100.80
WheelTimerDel                                    340.81%    30.35ms    32.95
----------------------------------------------------------------------------
PQTimerTick                                                 10.94ms    91.45
TreeTimerTick                                     61.18%    17.87ms    55.95
WheelTimerTick                                    47.65%    22.95ms    43.57
============================================================================
```


# 结论
 
* 相比之下，PerTick()最小堆性能消耗最少，CancelTimer()性能较低；
* 红黑树的AddTimer()和CancelTimer()效率都远高于另外另个实现；
* 时间轮整体表现不俗，PerTick部分如果再能优化，是平均效率最优秀的选择；
* 最小堆的编码实现最简单，大多数编程语言都可以迅速实现，实现难度：最小堆 < 时间轮 < 红黑树；


## TO-DO

* 时间轮实现还可以做内存优化
* 集成[其它benchmark框架](https://github.com/google/benchmark)来跑性能数据
