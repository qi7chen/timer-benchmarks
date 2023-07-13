# timer-benchmark

测试不同的数据结构（最小堆、四叉堆、红黑树、时间轮）实现的定时器的性能差异。

as [Hashed and Hierarchical Timing Wheels](http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf) implies

a timer module has 3 component routines:

``` C++
// start a timer that will expire after `interval` unit of time
// return an unique id of the pending timer
int Start(interval, expiry_action)

// cancel a timer identified by `timer_id`
void Cancel(timer_id)

// per-tick bookking routine
// in single-thread timer scheduler implementions, this routine will run timeout actions
int Tick(now)
```

use [min-heap](https://en.wikipedia.org/wiki/Heap_(data_structure)), quaternary heap( [4-ary heap](https://en.wikipedia.org/wiki/D-ary_heap) ),
balanced binary search tree( [red-black tree](https://en.wikipedia.org/wiki/Red-black_tree) ), [hashed timing wheel](https://netty.io/4.0/api/io/netty/util/HashedWheelTimer.html)
and [Hierarchical timing wheel](https://lwn.net/Articles/646950/) to implement different time scheduler.


## Big(O) complexity of algorithm


algo                    | Start()    | Cancel() | Tick()   | implemention file
--------------------------|-----------|---------|----------|--------------------------
binary heap               | O(log N) | O(log N) | O(1)     | [PriorityQueueTimer](src/PriorityQueueTimer.h)
4-ary heap                | O(log N) | O(log N) | O(1)     | [QuatHeapTimer](src/QuatHeapTimer.h)
redblack tree             | O(log N) | O(log N) | O(log N) | [RBTreeTimer](src/RBTreeTimer.h)
hashed timing wheel       | O(1)     | O(1)     | O(1)     | [HashedWheelTimer](src/HashedWheelTimer.h)
hierarchical timing wheel | O(1)     | O(1)     | O(1)     | [HHWheelTimer](src/HHWheelTimer.h)


## How To Build

### Obtain CMake

Obtain [CMake](https://cmake.org) first.

* `sudo apt install cmake` on ubuntu or debian
* `sudo yum install cmake` on redhat or centos
* `choco install cmake` on windows use [choco](https://chocolatey.org/)

run shell command

* `mkdir cmake-build; cd cmake-build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .`



## Benchmarks

## Benchmark result

AMD x64 6-core 3.93MHz CPU

Benchmark                 |       Time      | CPU     |  Iterations
--------------------------|-----------------|---------|--------------
BM_PQTimerAdd             |     423 ns      | 410 ns  |    1600000
BM_QuadHeapTimerAdd       |     399 ns      | 401 ns  |    1947826
BM_RBTreeTimerAdd         |    1181 ns      | 1186 ns |    1120000
BM_HashWheelTimerAdd      |     376 ns      | 377 ns  |    1947826
BM_HHWheelTimerAdd        |     430 ns      | 436 ns  |    1792000
                          |                 |         |
BM_PQTimerCancel          |     503 ns      | 500 ns  |    1000000
BM_HashWheelTimerCancel   |     443 ns      | 443 ns  |    1659259
BM_HHWheelTimerCancel     |     598 ns      | 578 ns  |    1000000
                          |                 |         |
BM_PQTimerTick            |     278 ns      | 276 ns  |    2488889
BM_QuadHeapTimerTick      |     294 ns      | 289 ns  |    2488889
BM_RBTreeTimerTick        |     324 ns      | 318 ns  |    2357895
BM_HashWheelTimerTick     |     337 ns      | 335 ns  |    2240000
BM_HHWheelTimerTick       |     293 ns      | 293 ns  |    2133333

## Conclusion
TODO:


## Reference

* [Hashed and Hierarchical Timing Wheels](https://paulcavallaro.com/blog/hashed-and-hierarchical-timing-wheels/)
* [Hashed and Hierarchical Timing Wheels](http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf)
* [Netty HashedWheelTimer](https://github.com/netty/netty/blob/4.1/common/src/main/java/io/netty/util/HashedWheelTimer.java)
* [Apache Kafka, Purgatory, and Hierarchical Timing Wheels](https://www.confluent.io/blog/apache-kafka-purgatory-hierarchical-timing-wheels/s)


