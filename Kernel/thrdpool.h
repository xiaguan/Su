//
// Created by root on 22-6-17.
//

#ifndef SU_THRDPOOL_H
#define SU_THRDPOOL_H

#include <stddef.h>

typedef struct __thrdpool thrdpool_t;

struct thrdpool_task
{
    void (*routine)(void *);
    void *context; // todo
};

/*
 *  I think one thing interested me is
 *  the thread pool can be destroyed by a thread task.
 *  Destroying thread pool in thread task does not end the task thread
 *  It'll run till the end of task
 */

#ifdef __cplusplus
extern "C"
{
#endif
        // todo: stacksize
/*
 * It's just like the construct func OOP
 * like thread_pool(size_t ntrheads,size_t stacksize) : xx() {xx}
 */
thrdpool_t *thrdpool_create(size_t nthreads,size_t stacksize);

/*
 * It's more like thread_pool.push_task(Task new_task) ,but C style
 * using Task = std::functional<void(void *)>;
 * void * arg ,allow us to pass data to the task.
 * void * is just like the template in Cpp
 * if we write cpp template style, emm....,I think it's pretty hard
 */
int thrdpool_schedule(const struct thrdpool_task *task, thrdpool_t *pool);
int thrdpool_increase(thrdpool_t *pool);
int thrdpool_in_pool(thrdpool_t *pool);
void thrdpool_destroy(void (*pending)(const struct thrdpool_task *),
        thrdpool_t *pool);

#ifdef __cplusplus
}
#endif

#endif //SU_THRDPOOL_H
