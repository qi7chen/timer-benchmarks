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
PQTimerAdd                                                  28.44ms    35.17
TreeTimerAdd                                      71.02%    40.04ms    24.97
WheelTimerAdd                                    293.08%     9.70ms   103.07
----------------------------------------------------------------------------
PQTimerDel                                                 850.45ms     1.18
TreeTimerDel                                       5.09%     16.71s   59.83m
WheelTimerDel                                    15.19K%     5.60ms   178.65
----------------------------------------------------------------------------
PQTimerTick                                                 21.11ms    47.38
TreeTimerTick                                     86.90%    24.29ms    41.17
WheelTimerTick                                    84.62%    24.94ms    40.09
============================================================================



```

```
i5 3.4GHz Windows 10 Visual C++ 15
============================================================================
test\benchtimer.cpp                             relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  12.27ms    81.51
TreeTimerAdd                                      54.89%    22.35ms    44.74
WheelTimerAdd                                     70.70%    17.35ms    57.63
----------------------------------------------------------------------------
PQTimerDel                                                 706.29ms     1.42
TreeTimerDel                                       5.12%     13.81s   72.43m
WheelTimerDel                                     8.07K%     8.75ms   114.23
----------------------------------------------------------------------------
PQTimerTick                                                  1.37ms   728.89
TreeTimerTick                                     38.55%     3.56ms   280.97
WheelTimerTick                                    30.36%     4.52ms   221.31
============================================================================
```


# 结论

* Windows和Linux两个平台有一些差异；
* 相比之下，最小堆PerTick()性能消耗最少，AddTimer()和CancelTimer()性能偏低；
* 红黑树的AddTimer()和CancelTimer()效率都低于另外另个实现；
* 时间轮整体表现不俗，PerTick部分如果再能优化，是平均效率最优秀的选择；
* 最小堆的编码实现最简单，大多数编程语言都可以迅速实现，实现难度：最小堆 < 时间轮 < 红黑树；


## TO-DO

* 集成[其它benchmark框架](https://github.com/google/benchmark)来跑性能数据
