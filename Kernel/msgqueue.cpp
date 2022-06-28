//
// Created by root on 22-6-26.
//

#include "msgqueue.h"

#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

struct __msgqueue
{
    size_t msg_max; // for producer not consumer
    size_t msg_cnt;
    int linkoff;
    int is_nonblock;
    void *head1;
    void *head2;
    void **get_head;
    void **put_head;
    void **put_tail;
    pthread_mutex_t  get_mutex;
    pthread_mutex_t  put_mutex;
    pthread_cond_t get_cond;
    pthread_cond_t put_cond;
};

/*
 * The main design idea is some tasks are very small
 * The queue is very busy , task in and out
 * But one main question is the producers and the consumers share one lock
 * So what the msgqueue do is to device the producer and consumer into two different queue
 * All producers share one lock,and all consumers share another lock
 * When consumers process all task done, it will switch the queue with producers
 *
 *
 * In my point of view, if we can add task-steal for nonblock queue.
 * If current pool has no job to do , it tries to steal other pool' tasks.
 * Obviously, to implementation this ,we need a class called queue_manager?
 * In another view, this method cause complexity. I will do some labs in the future.
 */


/*
 * This a very hard problem,
 * How to set a queue to be non_block ,when the queue is running?
 *
 * 消费者有如下几个状态：
 * 1. 取任务，会把锁锁上一小会
 * 2. 取不出来任务，进入到msgqueue_swap里，没有消息会等在while循环处
 * 也就是说get_mutex 存在长时间持锁行为
 * 而put_mutex没有，即使队列满了，它也是cond_wait
 * 或者在swap里，它并没有被长时间持有
 *
 * 为什么不锁住消费者？
 * 1. 有任务可取的时候，消费者的行为并不存在阻塞
 * 2. 无任务可取的时候，消费者等待在swap中等待get_cond，此时锁是锁了的
 */
void msgqueue_set_nonblock(msgqueue_t *queue){
    queue->is_nonblock = true;
    pthread_mutex_lock(&queue->put_mutex);
    pthread_cond_signal(&queue->get_cond);
    pthread_cond_broadcast(&queue->put_cond);
    pthread_mutex_unlock(&queue->put_mutex);
}

void msgqueue_set_block(msgqueue_t *queue)
{
    queue->is_nonblock = 0;
}

/*
 * create a msgqueue ,return the queue or nullptr
 */
msgqueue_t *msgqueue_create(size_t maxlen,int linkoff)
{
        __msgqueue * result = (__msgqueue*)malloc(sizeof(__msgqueue));\
        if(!result) return nullptr;

        int ret;
        ret = pthread_mutex_init(&result->get_mutex,nullptr);
        if(ret == 0)
        {
                ret = pthread_mutex_init(&result->put_mutex,nullptr);
                if(ret == 0)
                {
                        ret = pthread_cond_init(&result->get_cond, nullptr);
                        if(ret == 0)
                        {
                                ret = pthread_cond_init(&result->put_cond, nullptr);
                                if(ret == 0)
                                {
                                        result->linkoff = linkoff;
                                        result->is_nonblock = false;
                                        result->msg_max = maxlen;
                                        result->msg_cnt = 0;

                                        result->head1 = nullptr;
                                        result->head2 = nullptr;

                                        result->get_head = &result->head1;
                                        result->put_head = &result->head2;
                                        result->put_tail = &result->head2;

                                        return result;
                                }

                                errno = ret;
                                pthread_cond_destroy(&result->put_cond);
                        }

                        errno = ret;
                        pthread_cond_destroy(&result->get_cond);
                }
                errno = ret;
                pthread_mutex_destroy(&result->put_mutex);
        }
        pthread_mutex_destroy(&result->get_mutex);
        errno = ret;
        free(result);
        return nullptr;
}

void msgqueue_put(void * msg,msgqueue_t * queue)
{
        void **link = (void**)((char*)msg + queue->linkoff);

        *link = nullptr;
        pthread_mutex_lock(&queue->put_mutex);
        while(queue->msg_cnt > queue->msg_max -1 && !queue->is_nonblock)
                pthread_cond_wait(&queue->put_cond,&queue->put_mutex);

        *queue->put_tail = link;
        queue->put_tail = link;
        ++queue->msg_cnt;
        pthread_mutex_unlock(&queue->put_mutex);
        pthread_cond_signal(&queue->get_cond);
}

static size_t __msgqueue_swap(msgqueue_t *queue)
{
        void **get_head = queue->get_head;
        size_t cnt;

        queue->get_head = queue->put_head;
        pthread_mutex_lock(&queue->put_mutex);
        while(queue->msg_cnt == 0 && !queue->is_nonblock)
                pthread_cond_wait(&queue->get_cond,&queue->put_mutex);

        cnt = queue->msg_cnt;
        if(cnt > queue->msg_max -1)
                pthread_cond_broadcast(&queue->put_cond);

        queue->put_head = get_head;
        queue->put_tail = get_head;
        queue->msg_cnt = 0;
        pthread_mutex_unlock(&queue->put_mutex);
        return cnt;
}

void* msgqueue_get(msgqueue_t * queue)
{
        void * msg;

        pthread_mutex_lock(&queue->get_mutex);

        if(*queue->get_head || __msgqueue_swap(queue) > 0 ) {
                msg = (char *)*queue->get_head - queue->linkoff;
                *(queue->get_head) = *(void **)*queue->get_head;
        }
        else{
                msg = nullptr;
                errno = ENOENT;
        }
        pthread_mutex_unlock(&queue->get_mutex);
        return msg;
}

void msgqueue_destroy(msgqueue_t *queue)
{
	pthread_cond_destroy(&queue->put_cond);
	pthread_cond_destroy(&queue->get_cond);
	pthread_mutex_destroy(&queue->put_mutex);
	pthread_mutex_destroy(&queue->get_mutex);
	free(queue);
}