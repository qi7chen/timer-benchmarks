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
E5 2.3GHz CentOS 7 GCC 5.3
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
i5 3.4GHz Windows 10 Visual C++ 15
============================================================================
test\BenchTimer.cpp                              relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  12.21ms    81.90
TreeTimerAdd                                      54.30%    22.49ms    44.47
WheelTimerAdd                                     58.86%    20.75ms    48.20
----------------------------------------------------------------------------
PQTimerDel                                                 675.82ms     1.48
TreeTimerDel                                       5.17%     13.08s   76.48m
WheelTimerDel                                     6.40K%    10.56ms    94.68
----------------------------------------------------------------------------
PQTimerTick                                                  1.32ms   757.94
TreeTimerTick                                     40.09%     3.29ms   303.88
WheelTimerTick                                    21.02%     6.28ms   159.29
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
