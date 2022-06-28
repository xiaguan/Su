//
// Created by root on 22-6-17.
//

#include <Kernel/thrdpool.h>
#include <iostream>

#include <utils/util.h>
#include <pthread.h>

void test_func(void *arg)
{
        std::cout <<"task is running "<<std::endl;
        int out_value = *(int *)arg;
        std::cout <<"task is running "<<std::endl;
        for(int i = 10;i<2*1000*1000;i++){
                out_value += i;
                out_value -= 10;
                out_value += out_value/i;
        }
        free(arg);
}

void end_task(void * arg)
{
        auto timer = (su::TimeCounter*)arg;
        timer->end_and_cout();
}

int main()
{
        su::TimeCounter timeCounter;
        auto pool = thrdpool_create(15,0);
        for(int i = 100;i<101;i++){
                thrdpool_task *new_task = new thrdpool_task;
                new_task->routine = test_func;
                new_task->context = new int(i);
                std::cout << new_task->context << std::endl;
                thrdpool_schedule(new_task,pool);
        }
        thrdpool_task *new_task = new thrdpool_task;
        new_task->routine = end_task;
        new_task->context = &timeCounter;
        thrdpool_schedule(new_task,pool);
        int a;
        std::cin >> a;
}