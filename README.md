# 介绍
Su 是一个未完成的C++调度库，已完成内容：c语言的侵入式链表，消息队列（双队列实现），一个线程池，和一个基本任务框架设计，目前只支持线程任务。

同时在`/tests/Zipper`目录下有一个基于`Huffman`算法的多线程压缩解压类，使用`filename`和对应的解压，压缩类即可完成高效并发压缩解压缩操作。在SSD上，压缩吞吐可达`160mb/s`,解压吞吐可达`30mb/s`。