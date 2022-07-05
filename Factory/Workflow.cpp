//
// Created by root on 22-6-21.
//

#include "Workflow.h"

SeriesWork::SeriesWork(SubTask *first, series_callback_t &&callback)
:first(first)
{
    this->queue = buf;

    assert(!series_of(first));
    this->first->set_pointer(this);
}

SeriesWork::~SeriesWork()
{
    if(this->queue != this->buf)
    {
        delete [] queue;
    }
}

void SeriesWork::expand_queue() {
    int size = 2 * this->queue_size;
    SubTask **new_queue = new SubTask*[size];

    int i = 0,j = this->front;

    do{
        new_queue[i++]  = this->queue[j++];
        if(j == queue_size)
            j = 0;
    }while(j != this->back);

    this->queue = new_queue;
    this->queue_size = size;
    this->front = 0;
    this->back = i;
}

void SeriesWork::push_back(SubTask *task) {
    this->mutex.lock();
    task->set_pointer(this);
    this->queue[this->back] = task;

    /*
     * 循环利用
     */
    if(++this->back == this->queue_size)
        this->back = 0;
    if(this->front == this->back)
        this->expand_queue();
    this->mutex.unlock();
}

void SeriesWork::push_front(SubTask *task) {
    this->mutex.lock();
    task->set_pointer(this);

    if(--this->front < 0)
        this->front = this->queue_size - 1;
    this->queue[this->front] = task;
    if(this->front == this->back)
        expand_queue();
    this->mutex.unlock();
}

SubTask* SeriesWork::pop() {
    bool taskState = this->canceled;
    SubTask *task = this->pop_task();

    if(!canceled)
        return task;

    while(task)
    {
        delete task;
        task = this->pop_task();
    }

    return nullptr;
}

SubTask *SeriesWork::pop_task() {
    SubTask *task;

    this->mutex.lock();
    if(this->front != this->back)
    {
        task = this->queue[this->front];
        if( ++this->front == this->queue_size)
        {
            front = 0;
        }
    }
    else
    {
        task = this->last;
        this->last = nullptr;
    }

    this->mutex.unlock();

    if(task == nullptr)
    {
        this->finished = true;

        if(this->callback)
        {
            this->callback(this);
        }

        if(!this->in_parallel)
            delete this;
    }

    return task;
}

void SeriesWork::dismiss_recursive() {
    SubTask *task = first;

    this->callback = nullptr;
    do{
        delete task;
        task = this->pop_task();
    } while (task);
}
