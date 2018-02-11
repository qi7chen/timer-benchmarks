# BenchmarkTimers

分别使用最小堆、红黑树、时间轮实现定时器，再对插入、删除、循环做性能测试。


# 如何构建本项目

* 下载[premake](https://premake.github.io/download.html#v5)
* 使用premake生成Visual Studio解决方案或者makefile，如`premake5 vs2017`


# 性能测试

## 算法复杂度

复杂度比较：

```
algo   | Add()    | Cancel() | Tick()   | impl
--------------------------------------------------------
最小堆 | O(log N) | O(log N) | O(1)     | src/PQTimer.h
红黑树 | O(log N) | O(log N) | O(log N) | src/TreeTimer.h
时间轮 | O(1)     | O(1)     | O(1)     | src/WheelTimer.h
```


在一台 i3 4 Core 1.9GHz的Windows 7上的测试数据:

```
============================================================================
test\BenchTimer.cpp                              relative  time/iter  iters/s
============================================================================
TreeTimerAdd                                                 6.68us  149.60K
PQTimerAdd                                       106.46%     6.28us  159.26K
WheelTimerAdd                                     22.92%    29.17us   34.28K
----------------------------------------------------------------------------
TreeTimerDel                                                 4.03us  247.89K
PQTimerDel                                       120.71%     3.34us  299.23K
WheelTimerDel                                     13.89%    29.03us   34.44K
----------------------------------------------------------------------------
TreeTimerTick                                                3.14us  318.55K
PQTimerTick                                       98.41%     3.19us  313.48K
WheelTimerTick                                    10.66%    29.44us   33.97K
============================================================================
```


测试表示红黑树跟最小堆只有不到平均10%的性能差距，而时间轮的性能只有红黑树的20%，
或许在于我的时间轮没写对或者优化做的还不够好？

## TO-DO

* 内存池优化