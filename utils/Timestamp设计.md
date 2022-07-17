# 基于C++ chrono 的Timestamp实现
因为考虑到以后可能会用自己的库写分布式数据库，日志的性能可用性很重要，日志很重要的
一个东西就是时间戳，因为每一条日志都需要时间戳。加之C style的时间并不是很习惯，所以参考了
很多资料设计了这个Timestamp类。
## chrono介绍
 chrono里有三个概念，clock,time_point,duration。clock包括了system_clock,steady_clock,
 highsolution什么，最后这个其实是个typedef至少g++ msvc都是这么干的，g++是system_clock,而msvc
 是steady_clock。
### time_point
它是通过调用clock的now返回的，翻译过来就是时间点，实际上也可以看作duration,因为其本质上是公元某一天
1月1号，到现在过了多久。所以它有一个成员函数叫做time_since_epoch()，用于转换为duration。
### duration
表示一个时间段，花费了多少时间。需要指定存储大小和精度（小时，秒，纳秒...），一般不用你自己手动指定模板参数。
一般使用比如:std::chrono::seconds这样来获取对应精度的duration类型。另外有一个重要的函数就是duration_cast，
使用它将某个精度的duration转换为另一个精度duration,便于信息获取。

## 设计旅途
首先时间的获取，我们选择system_clock::now()来获取时间，但是返回值是个time_point，我怎么将其转换为
字符串呢？一开始，看到了C style的time_t那一套，也不是不行，但是精度在秒，而且转换来转换去的，我感觉很奇怪。
后面使用了20标准的std::chrono::hh_mm_ss，用于将一个duration拆分为小时，分，秒，更小的精度，这个东西用一次可以，
但是操作起来非常不方便。折腾了很久，包括时间刷新参考了muduo的每秒刷一次，其他只会刷新毫秒部分。
多的也不说，上代码更直观，包括测试,linux下测试结果，比muduo略快。

测试位于test目录下