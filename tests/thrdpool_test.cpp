//
// Created by root on 22-6-17.
//

#include <Kernel/thrdpool.h>
#include <iostream>

#include <pthread.h>
pthread_mutex_t  mutex;
void test_func(void *arg)
{
        int out_value = *(int *)arg;
        for(int i = 10;i<20;i++){
                out_value += i;
        }
        free(arg);
}

void end_task(void * arg)
{
        std::cout <<"get a end task"<<std::endl;
        thrdpool_destroy(NULL,(thrdpool_t * )arg);
}

int main()
{
        auto pool = thrdpool_create(15,0);
        pthread_mutex_init(&mutex,NULL);
        for(int i = 0;i<100;i++){
                thrdpool_task *new_task = new thrdpool_task;
                new_task->routine = test_func;
                new_task->context = new int(i);
                thrdpool_schedule(new_task,pool);
        }
//        thrdpool_task *new_task = new thrdpool_task;
//        new_task->routine = end_task;
//        new_task->context = pool;
//    thrdpool_schedule(new_task,pool);
    thrdpool_destroy(NULL,pool);
    int a;
        std::cin >> a;
}