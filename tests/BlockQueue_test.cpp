//
// Created by susun on 2022/6/12.
//
#include <Thread/BlockQueue.h>

#include <iostream>
#include <atomic>
#include <future>

#include <Log/log.h>
#include <utils/util.h>

using namespace std;

/*
 * 测试主要分两方面： 一方面是对队列长度的限制是否成功，另一方面是是否任务都能正确取出。
 */

void max_lenth_test()
{
    su::log::Logger::ptr logger(new su::log::Logger("thread_test"));
    std::async()
    logger->setFileAppender("block_queue.txt");
    vector<thread> threads;
    su::TimeCounter timeCounter;
    BlockQueue<int> test_queue(40);
    atomic<int> count{0};
    const int thread_num = 10;
    auto worker_thread = [&test_queue,&count,&logger](){
        while(true){
            auto t = test_queue.get_front();
            //SU_LOG_DEBUG(logger) <<"  "<<t;
            if(t == - 1) return;
            ++count;
        }
    };

    for(int i = 0;i<10;i++) threads.emplace_back(worker_thread);

    for(int i = 0;i<10000;i++) test_queue.push(i);

    // 向每一个线程发送end Task
    for(int i = 0;i<thread_num;i++) test_queue.push(-1);
    for(auto & t : threads) t.join();
    timeCounter.end_and_cout();
    std::cout << count <<std::endl;
}
int main(){
    max_lenth_test();
}
