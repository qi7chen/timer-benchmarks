# timerqueue-benchmark

分别使用最小堆、红黑树、时间轮实现定时器，再用随机数据对插入、删除、循环做性能测试。

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
PQTimerAdd                                                  28.95ms    34.54
TreeTimerAdd                                      72.39%    40.00ms    25.00
WheelTimerAdd                                    190.25%    15.22ms    65.71
----------------------------------------------------------------------------
PQTimerDel                                                 873.13ms     1.15
TreeTimerDel                                       5.23%     16.69s   59.92m
WheelTimerDel                                     9.66K%     9.04ms   110.61
----------------------------------------------------------------------------
PQTimerTick                                                 21.50ms    46.51
TreeTimerTick                                     86.37%    24.89ms    40.17
WheelTimerTick                                    73.83%    29.12ms    34.34
============================================================================


```

```
i3 1.9GHz Windows 7
============================================================================
test\BenchTimer.cpp                              relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  20.99ms    47.65
TreeTimerAdd                                      51.84%    40.48ms    24.70
WheelTimerAdd                                     52.23%    40.18ms    24.89
----------------------------------------------------------------------------
PQTimerDel                                                    1.88s  531.21m
TreeTimerDel                                       5.91%     31.85s   31.40m
WheelTimerDel                                     8.64K%    21.80ms    45.88
----------------------------------------------------------------------------
PQTimerTick                                                  2.10ms   477.26
TreeTimerTick                                     24.40%     8.59ms   116.45
WheelTimerTick                                    16.23%    12.91ms    77.46
============================================================================
```


# 结论

* Windows和Linux两个平台有一些差异；
* 相比之下，最小堆PerTick()性能消耗最少，AddTimer()和CancelTimer()性能偏低；
* 红黑树的AddTimer()和CancelTimer()效率都低于另外另个实现；
* 时间轮整体表现不俗，PerTick部分如果再能优化，是平均效率最优秀的选择；
* 最小堆的编码实现最简单，大多数编程语言都可以迅速实现，实现难度：最小堆 < 时间轮 < 红黑树；


## TO-DO

* 时间轮实现还可以做内存优化
* 集成[其它benchmark框架](https://github.com/google/benchmark)来跑性能数据
