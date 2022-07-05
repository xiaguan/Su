//
// Created by root on 22-6-19.
//

#include "Executor.h"
#include "thrdpool.h"
#include "list.h"

#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

struct ExecTaskEntry
{
    struct list_head list;
    ExecSession *session;
    thrdpool_t *thrdpool;
};

int ExecQueue::init()
{
        int ret;

        ret = pthread_mutex_init(&this->mutex,nullptr);
        if(ret == 0)
        {
                INIT_LIST_HEAD(&this->task_list);
                return 0;
        }

        errno = ret;
        return -1;
}

void ExecQueue::deinit()
{
        pthread_mutex_destroy(&this->mutex);
}

int Executor::init(size_t nthreads)
{
        if(nthreads == 0)
        {
                errno = EINVAL;
                return -1;
        }
        this->thrdpool = thrdpool_create(nthreads,0);
        if(this->thrdpool)
                return 0;
        return -1;
}

void Executor::deinit()
{
        thrdpool_destroy(Executor::excutor_cancle_tasks,this->thrdpool);
}

extern "C" void __thrdpool_schedule(const struct thrdpool_task *task, void *buf,
                                      thrdpool_t *pool);

void Executor::excutor_thread_routine(void *context)
{
        ExecQueue *queue = (ExecQueue *)context;
        struct ExecTaskEntry *entry;
        ExecSession *session;

        pthread_mutex_lock(&queue->mutex);
        entry = list_entry(queue->task_list.next,struct ExecTaskEntry, list);
        list_del(&entry->list);
        session = entry->session;
        if(!list_empty(&queue->task_list))
        {
                struct thrdpool_task task = {
                        .routine = Executor::excutor_thread_routine,
                        .context = queue
                };
                __thrdpool_schedule(&task,entry,entry->thrdpool);
        }
        else
                free(entry); //todo
        pthread_mutex_unlock(&queue->mutex);

        session->execute();
        session->handle(ES_STATE_FINISHED,0);
}

void Executor::excutor_cancle_tasks(const struct thrdpool_task *task)
{
    ExecQueue *queue =  (ExecQueue*)task->context; //todo
    struct ExecTaskEntry *entry;
    struct list_head *pos,*tmp;
    ExecSession *session;

    list_for_each_safe(pos,tmp,&queue->task_list)
    {
            entry = list_entry(pos, struct ExecTaskEntry,list);
            list_del(pos);
            session = entry->session;
            free(entry);

            session->handle(ES_STATE_CANCELED,0);
    }
}


int Executor::request(ExecSession *session, ExecQueue *queue)
{
        struct ExecTaskEntry *entry;

        // the only way to set a session's queue ?
        session->queue = queue;
        // make new ExecTask
        entry = (struct ExecTaskEntry *) malloc(sizeof(struct ExecTaskEntry));
        if(entry)
        {
                entry->session = session;
                entry->thrdpool = this->thrdpool;
                pthread_mutex_lock(&queue->mutex);
                /* push task */
                list_add_tail(&entry->list,&queue->task_list);
                if(queue->task_list.next == &entry->list)
                {
                        // the first task of the queue
                        struct thrdpool_task task = {
                                .routine = Executor::excutor_thread_routine,
                                .context = queue
                        };
                        if(thrdpool_schedule(&task,this->thrdpool) < 0 )
                        {
                                list_del(&entry->list);
                                free(entry);
                                entry = NULL;
                        }
                }
                /*
                 * if the Task is not the first task of ExecQueue,
                 * it'll be got in Executor::exec_thread_routine
                 */
                pthread_mutex_unlock(&queue->mutex);
        }

        return -!entry;
}