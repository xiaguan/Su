//
// Created by susun on 2022/6/28.
//

#include <Kernel/msgqueue.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <Kernel/thrdpool.h>
#include <utils/util.h>

double result;
std::atomic<int> cnt(0);

void task(void * arg)
{
    int out_value = *(int *)arg;
    for(int i = 10;i<2*1000*1000;i++){
        out_value += i;
        out_value -= 10;
        out_value += out_value/i;
    }
    result += out_value;
    cnt++;
    free(arg);
}

void end_task(void * arg)
{
    auto timer = (su::TimeCounter*)arg;
    timer->end_and_cout();
}

int main(){
    su::TimeCounter timeCounter;
    auto pool = thrdpool_create(15,0);
    for(int i = 0;i<1000;i++)
    {
        auto new_task = new thrdpool_task;
        new_task->routine = task;
        new_task->context = new int(i);
        thrdpool_schedule(new_task,pool);
    }
    thrdpool_task *new_task = new thrdpool_task;
    new_task->routine = end_task;
    new_task->context = &timeCounter;
    thrdpool_schedule(new_task,pool);
    int a;
    std::cin >> a;
    std::cout << cnt <<std::endl;
}