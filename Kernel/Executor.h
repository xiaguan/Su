//
// Created by root on 22-6-19.
//

#ifndef SU_EXECUTOR_H
#define SU_EXECUTOR_H

#include <stddef.h>
#include <pthread.h>
#include "list.h"

/*
 * About the Execute-module
 * Each executor hold a threadpool, do not share
 * The executor doesn't have a ExecQueue
 * We add task to the executor by use the interface " int request(ExecQueue,ExecSession)
 * That makes a task joined in the queue
 * The executor will not only execute the task, but also the whole queue.
 *
 * About the ExecQueue, it's just a task queue.
 * One more thing about it is, in fact, it ofter comes with queue_name.
 * It just a way to find the queue by using unordered_map<std::string,ExecQueue*>
 *
 * ExecSession is just Task , by it is an abstract class.
 * Before we use , we first need to inherit it,and then override the execute()
 * execute() is the task routine.
 *
 *
 * So the flow of start a task is like this
 * Inherit the ExecSession , add more data or method to you own task class
 * Then if task queue is not so import for you
 * Just new one ,and new a executor , use executor->request(queue,session)
 * The executor with make these task to be thrdpool_task and add it to its own thrdpool's taskqueue
 */

class ExecQueue
{
public:
    friend class Executor;

    int init();
    void deinit();

    virtual  ~ExecQueue() { }

private:
    struct list_head task_list;
    pthread_mutex_t mutex;
};

constexpr int ES_STATE_FINISHED = 0;
constexpr int ES_STATE_ERROR = 1;
constexpr int ES_STATE_CANCELED = 2;

class ExecSession
{
public:
    friend class Executor;

    // todo: the interface of execu
    virtual void execute() = 0;
    // todo : the interface of handle state
    virtual void handle(int state,int error) = 0;
    virtual ~ExecSession() { }


protected:
    ExecQueue *get_queue() {return this->queue;}

private:
    ExecQueue *queue; // todo: a ptr not a obj
};


class Executor
{
public:
    int init(size_t nthreads);
    void deinit();

    int request(ExecSession *session, ExecQueue *queue);

    virtual ~Executor() { }

private:
    struct __thrdpool *thrdpool;

private:
    static void excutor_thread_routine(void *context);
    static void excutor_cancle_tasks(const struct thrdpool_task *task);

};




#endif //SU_EXECUTOR_H
