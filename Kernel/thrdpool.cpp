//
// Created by root on 22-6-17.
//

#include "thrdpool.h"
#include "list.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

//todo
#include <iostream>

struct __thrdpool
{
    struct list_head task_queue;
    size_t nthreads;
    size_t statcksize;
    pthread_t tid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_key_t key;
    pthread_cond_t *terminate;
};

/*
 * make a task list ,like std::list<Task>
 */
struct __threadpoll_task_entry
{
    struct list_head list;
    struct thrdpool_task task;
};

static pthread_t __zero_tid;


/* It's what the worker thread do */
static void *__thrdpool_routine(void *arg)
{
        /*
         * in fact the the type of the arg is always thrdpool_t *
         * but create new thread could be task,too
         * change this to void* , so that we can make this func to be a task
         */
        thrdpool_t *pool = (thrdpool_t*)arg;
        struct list_head **pos = &(pool->task_queue.next);
        struct __threadpoll_task_entry *entry;
        void (*task_routine)(void *);
        void *task_context;
        pthread_t tid;

        /* pool is the private data of the key */
        pthread_setspecific(pool->key,pool);
        while(1)
        {
                // lock the mutex avoid data race
               pthread_mutex_lock(&pool->mutex);

               // the pool is running ,wait for the new task
               while(!pool->terminate && list_empty(&pool->task_queue))
                       pthread_cond_wait(&pool->cond,&pool->mutex);

               if(pool->terminate)
                       break;

               entry = list_entry(*pos,struct __threadpoll_task_entry,list);
               list_del(*pos);
               pthread_mutex_unlock(&pool->mutex);

               task_routine = entry->task.routine;
               task_context = entry->task.context;
               free(entry);  // task is new by user,delete by thredpool
               task_routine(task_context); //execute the new task

               if(pool->nthreads == 0)
               {
                   /* Thread pool was destroyed by the task. */
                       free(pool);
                       return NULL;
               }
        }

        /* todo:one thread joins another, Don't need to keep all thread IDs. */
        std::cout <<"tid is "<<tid <<" pool->tid is"<<pool->tid<<" "<<pthread_self()<<std::endl;
        tid = pool->tid;
        pool->tid = pthread_self();
        if(--pool->nthreads == 0)
                pthread_cond_signal(pool->terminate);

        pthread_mutex_unlock(&pool->mutex);
        // todo : why not use tid != __zero_tid
        if(memcmp(&tid,&__zero_tid,sizeof(pthread_t)) != 0)
                pthread_join(tid,NULL);
        else std::cout <<"hh"<<" "<<tid<<std::endl;
        return NULL;
}

/* init the mutex and cond of the pool */
static int __thrdpool_init_locks(thrdpool_t *pool)
{
        int ret;

        ret = pthread_mutex_init(&pool->mutex,NULL);
        if(ret == 0)
        {
                ret = pthread_cond_init(&pool->cond,NULL);
                if(ret == 0 )
                        return 0;

                pthread_mutex_destroy(&pool->mutex);
        }
        errno = ret;
        return -1;
}

static void __thrdpool_destroy_locks(thrdpool_t *pool)
{
	pthread_mutex_destroy(&pool->mutex);
	pthread_cond_destroy(&pool->cond);
}

static void __thrdpool_terminate(int in_pool, thrdpool_t *pool)
{
        //  cond in the stack memory init way
        pthread_cond_t term = PTHREAD_COND_INITIALIZER;
        pool->terminate = &term;
        pthread_cond_broadcast(&pool->cond);

        if(in_pool)
        {
                /* Thread pool destroyed in a pool thread is legal */
                std::cout <<"error"<<std::endl;
                pthread_detach(pthread_self());
                pool->nthreads--;
        }

        while(pool->nthreads > 0)
                pthread_cond_wait(&term,&pool->mutex);

        pthread_mutex_unlock(&pool->mutex);
       	if (memcmp(&pool->tid, &__zero_tid, sizeof (pthread_t)) != 0)
       		pthread_join(pool->tid, NULL);
}

static int __thrdpool_create_threads(size_t nthreads, thrdpool_t *pool)
{
        pthread_attr_t attr;
        pthread_t tid;
        int ret;

        ret = pthread_attr_init(&attr);
        if(ret == 0)
        {
                if(pool->statcksize)
                        pthread_attr_setstacksize(&attr,pool->statcksize);

                while(pool->nthreads < nthreads)
                {
                        ret = pthread_create(&tid,&attr,__thrdpool_routine,pool);
                        if(ret == 0)
                                pool->nthreads++;
                        else
                                break;
                }

                pthread_attr_destroy(&attr);
                if(pool->nthreads == nthreads)
                        return 0;

                __thrdpool_terminate(0,pool);
        }
        errno = ret;
        return -1;
}

thrdpool_t *thrdpool_create(size_t nthreads,size_t stacksize)
{
        thrdpool_t *pool;
        int ret;

        pool = (thrdpool_t *)malloc(sizeof(thrdpool_t));
        if(pool)
        {
                if(__thrdpool_init_locks(pool) >= 0)
                {
                        ret = pthread_key_create(&pool->key,NULL);
                        if(ret == 0)
                        {
                                INIT_LIST_HEAD(&pool->task_queue);
                                pool->statcksize = stacksize;
                                pool->nthreads = 0;
                                // if pool->tid == 0 ,it's a pool,not a thread
                                memset(&pool->tid,0,sizeof(pthread_t));
                                pool->terminate = NULL;
                                if(__thrdpool_create_threads(nthreads,pool) >= 0)
                                        return pool;

                                pthread_key_delete(pool->key);
                        }
                        else
                                errno = ret;

                        __thrdpool_destroy_locks(pool);
                }

                free(pool);
        }
        return NULL;
}

inline void __thrdpool_schedule(const struct thrdpool_task *task, void *buf,
                                        thrdpool_t *pool);
void __thrdpool_schedule(const struct thrdpool_task *task, void *buf,
                                        thrdpool_t *pool)
{
        struct __threadpoll_task_entry *entry = (struct __threadpoll_task_entry *)buf;

        entry->task = *task;
       	pthread_mutex_lock(&pool->mutex);
       	list_add_tail(&entry->list, &pool->task_queue);
       	pthread_cond_signal(&pool->cond);
       	pthread_mutex_unlock(&pool->mutex);
}

int thrdpool_schedule(const struct thrdpool_task *task, thrdpool_t *pool)
{
        void *buf = malloc(sizeof(struct __threadpoll_task_entry));

        if(buf)
        {
                __thrdpool_schedule(task,buf,pool);
                return 0;
        }
        return -1;
}

int thrdpool_increase(thrdpool_t *pool)
{
        pthread_attr_t attr;
        pthread_t tid;
        int ret;

        ret = pthread_attr_init(&attr);
        if(ret == 0)
        {
                if(pool->statcksize)
                        pthread_attr_setstacksize(&attr,pool->statcksize);

                pthread_mutex_lock(&pool->mutex);
                ret = pthread_create(&tid, &attr, __thrdpool_routine, pool);
                if (ret == 0)
                        pool->nthreads++;

                pthread_mutex_unlock(&pool->mutex);
                pthread_attr_destroy(&attr);
                if (ret == 0)
                        return 0;
        }
        errno = ret;
       	return -1;
}

inline int thrdpool_in_pool(thrdpool_t *pool)
{
        // todo
        return pthread_getspecific(pool->key) == pool;
}

void thrdpool_destroy(void (*pending)(const struct thrdpool_task*),thrdpool_t *pool)
{
        int in_pool = thrdpool_in_pool(pool);
        std::cout <<"in_pool"<<in_pool<<std::endl;
        struct __threadpoll_task_entry *entry;
        struct list_head *pos, *tmp;

        __thrdpool_terminate(in_pool,pool);
        list_for_each_safe(pos,tmp,&pool->task_queue)
        {
                entry = list_entry(pos,struct __threadpoll_task_entry, list);
                list_del(pos);
                if(pending)
                        pending(&entry->task);
                free(entry);
        }

        pthread_key_delete(pool->key);
        __thrdpool_destroy_locks(pool);
        if(!in_pool)
                free(pool);
}

