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
PQTimerAdd                                                  50.51ms    19.80
TreeTimerAdd                                      68.42%    73.82ms    13.55
WheelTimerAdd                                    207.81%    24.30ms    41.14
----------------------------------------------------------------------------
PQTimerDel                                                  10.27ms    97.40
TreeTimerDel                                     198.55%     5.17ms   193.40
WheelTimerDel                                    110.44%     9.30ms   107.57
----------------------------------------------------------------------------
PQTimerTick                                                 41.57ms    24.06
TreeTimerTick                                     90.57%    45.90ms    21.79
WheelTimerTick                                    82.97%    50.10ms    19.96
============================================================================
```

```
i3 1.9GHz Windows 7
============================================================================
test\BenchTimer.cpp                              relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  30.15ms    33.17
TreeTimerAdd                                      61.09%    49.35ms    20.26
WheelTimerAdd                                     41.45%    72.73ms    13.75
----------------------------------------------------------------------------
PQTimerDel                                                  43.77ms    22.85
TreeTimerDel                                     416.63%    10.51ms    95.19
WheelTimerDel                                    147.84%    29.60ms    33.78
----------------------------------------------------------------------------
PQTimerTick                                                 10.87ms    91.98
TreeTimerTick                                     61.05%    17.81ms    56.15
WheelTimerTick                                    50.78%    21.41ms    46.71
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
