# timerqueue-benchmark

as [Hashed and Hierarchical Timing Wheels](http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf) implies

a timer module has 3 component routines:

``` C++
// start a timer that will expire after `interval` unit of time
int Start(interval, expiry_action)

// use `tiemr_id` to locate a timer and stop it
void Stop(timer_id)

// per-tick bookking routine
int Tick(now)
```

use [min-heap](https://en.wikipedia.org/wiki/Heap_(data_structure)), quaternary heap or [4-ary heap](https://en.wikipedia.org/wiki/D-ary_heap), balanced binary search tree or [red-black tree](https://en.wikipedia.org/wiki/Red-black_tree), hashed timing wheel 
and Hierarchical timing wheel to model different time module 

分别使用最小堆、四叉堆、红黑树、时间轮、层级时间轮实现定时器，测试插入、删除、Tick操作的性能。

## Big(O) complexity of algorithm


algo                    | Start()    | Stop() | Tick()   | implemention file
--------------------------|-----------|---------|----------|--------------------------
binary heap               | O(log N) | O(log N) | O(1)     | [PriorityQueueTimer](src/PriorityQueueTimer.h)
4-ary heap                | O(log N) | O(log N) | O(1)     | [QuatHeapTimer](src/QuatHeapTimer.h)
redblack tree             | O(log N) | O(log N) | O(log N) | [RBTreeTimer](src/RBTreeTimer.h)
hashed timing wheel       | O(1)     | O(1)     | O(1)     | [HashedWheelTimer](src/HashedWheelTimer.h)
hierarchical timing wheel | O(1)     | O(1)     | O(1)     | [HHWheelTimer](src/HHWheelTimer.h)


# How To Build

### Obtain CMake

Obtain[CMake](https://cmake.org/download/) first.


### Linux or [WSL](https://docs.microsoft.com/en-us/windows/wsl/install)

* `make build`

or 

* `cmake -Bbuilds -DCMAKE_BUILD_TYPE=Release`
* `cmake --build builds --config Release`

### Windows

Visual C++ Compiler toolchain is necessary.



# Benchmarks

## Benchmark result


## Conclusion




## Reference

* [Hashed and Hierarchical Timing Wheels](https://paulcavallaro.com/blog/hashed-and-hierarchical-timing-wheels/)
* [Hashed and Hierarchical Timing Wheels](http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf)
* [Netty HashedWheelTimer](https://github.com/netty/netty/blob/4.1/common/src/main/java/io/netty/util/HashedWheelTimer.java)
* [Apache Kafka, Purgatory, and Hierarchical Timing Wheels](https://www.confluent.io/blog/apache-kafka-purgatory-hierarchical-timing-wheels/s)


