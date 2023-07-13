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

FIFO means whether same deadline timers expire in FIFO order.

algo                     | Start()    | Cancel() | Tick()   |  FIFO  | implemention file
--------------------------|-----------|---------|----------|---------|-----------------
binary heap               | O(log N) | O(log N) | O(1)     |   no   | [PriorityQueueTimer](src/PriorityQueueTimer.h)
4-ary heap                | O(log N) | O(log N) | O(1)     |   no   | [QuatHeapTimer](src/QuatHeapTimer.h)
redblack tree             | O(log N) | O(log N) | O(log N) |   no   | [RBTreeTimer](src/RBTreeTimer.h)
hashed timing wheel       | O(1)     | O(1)     | O(1)     |   yes  | [HashedWheelTimer](src/HashedWheelTimer.h)
hierarchical timing wheel | O(1)     | O(1)     | O(1)     |   yes  | [HHWheelTimer](src/HHWheelTimer.h)


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

Win10 x64 6-core 3.93MHz CPU


Benchmark               |        Time     |       CPU  | Iterations
------------------------|-----------------|------------|---------------
BM_PQTimerAdd           |      441 ns     |    433 ns  |   1947826
BM_QuadHeapTimerAdd     |      429 ns     |    427 ns  |   1866667
BM_RBTreeTimerAdd       |     1231 ns     |   1228 ns  |   1120000
BM_HashWheelTimerAdd    |      430 ns     |    436 ns  |   1792000
BM_HHWheelTimerAdd      |      669 ns     |    672 ns  |   1000000
BM_PQTimerCancel        |      668 ns     |    656 ns  |   1000000
BM_QuadHeapTimerCancel  |      351 ns     |    349 ns  |   2240000
BM_RBTreeTimerCancel    |     1685 ns     |   1692 ns  |    896000
BM_HashWheelTimerCancel |      632 ns     |    641 ns  |   1000000
BM_HHWheelTimerCancel   |      942 ns     |    953 ns  |   1000000
BM_PQTimerTick          |     29.8 ns     |   29.8 ns  |  23578947
BM_QuadHeapTimerTick    |     30.3 ns     |   30.5 ns  |  23578947
BM_RBTreeTimerTick      |     30.2 ns     |   29.8 ns  |  23578947
BM_HashWheelTimerTick   |     31.2 ns     |   30.8 ns  |  21333333
BM_HHWheelTimerTick     |     30.5 ns     |   30.7 ns  |  22400000

## Conclusion

* rbtree timer Add/Cancel has not so good performance compare to other implementations.
* binary min heap is a good choice, easy to implement and have a good performance, but without FIFO expiration order .


## Reference

* [Hashed and Hierarchical Timing Wheels](https://paulcavallaro.com/blog/hashed-and-hierarchical-timing-wheels/)
* [Hashed and Hierarchical Timing Wheels](http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf)
* [Netty HashedWheelTimer](https://github.com/netty/netty/blob/4.1/common/src/main/java/io/netty/util/HashedWheelTimer.java)
* [Apache Kafka, Purgatory, and Hierarchical Timing Wheels](https://www.confluent.io/blog/apache-kafka-purgatory-hierarchical-timing-wheels/s)


