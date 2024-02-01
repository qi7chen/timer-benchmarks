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
int Update(now)
```

use [min-heap](https://en.wikipedia.org/wiki/Heap_(data_structure)), quaternary heap( [4-ary heap](https://en.wikipedia.org/wiki/D-ary_heap) ),
balanced binary search tree( [red-black tree](https://en.wikipedia.org/wiki/Red-black_tree) ), [hashed timing wheel](https://netty.io/4.0/api/io/netty/util/HashedWheelTimer.html)
and [Hierarchical timing wheel](https://lwn.net/Articles/646950/) to implement different time scheduler.


## Big(O) complexity of algorithm

FIFO means whether same deadline timers expire in FIFO order.

FIFO的意思是相同到期时间的定时器是否按FIFO的顺序到期

algo                      |          | Start()  | Cancel() | Tick()   |  FIFO  | implemention file
--------------------------|----------|----------|----------|----------|--------|-----------------------
binary heap               | 最小堆   | O(log N) | O(log N) | O(1)     |   no   | [PriorityQueueTimer](src/PriorityQueueTimer.h)
4-ary heap                | 四叉堆   | O(log N) | O(log N) | O(1)     |   no   | [QuatHeapTimer](src/QuatHeapTimer.h)
redblack tree             | 红黑树   | O(log N) | O(log N) | O(log N) |   no   | [RBTreeTimer](src/RBTreeTimer.h)
hashed timing wheel       | 时间轮   | O(1)     | O(1)     | O(1)     |   yes  | [HashedWheelTimer](src/HashedWheelTimer.h)
hierarchical timing wheel | 多级时间轮 | O(1)   | O(1)     | O(1)     |   yes  | [HHWheelTimer](src/HHWheelTimer.h)


## How To Build

### Obtain CMake

Obtain [CMake](https://cmake.org) first.

* `sudo apt install cmake` on Ubuntu or Debian
* `sudo yum install cmake` on Redhat or CentOS
* `choco install cmake` on Windows use [choco](https://chocolatey.org/)

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

* rbtree timer Add/Cancel has not so good performance compare to other implementations;
* 红黑树的插入和删除相比其它实现，表现都弱了一些；
* binary min heap is a good choice, easy to implement and have a good performance, but without FIFO expiration order(heap sort is unstable);
* 最小堆是一个不错的选择，代码实现简单性能也不俗，但不支持相同超时的定时器按FIFO顺序触发;


## Reference

* [Hashed and Hierarchical Timing Wheels](https://paulcavallaro.com/blog/hashed-and-hierarchical-timing-wheels/)
* [Hashed and Hierarchical Timing Wheels](http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf)
* [Netty HashedWheelTimer](https://github.com/netty/netty/blob/4.1/common/src/main/java/io/netty/util/HashedWheelTimer.java)
* [Apache Kafka, Purgatory, and Hierarchical Timing Wheels](https://www.confluent.io/blog/apache-kafka-purgatory-hierarchical-timing-wheels/s)


