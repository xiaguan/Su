//
// Created by root on 22-6-21.
//

#ifndef SU_WORKFLOW_H
#define SU_WORKFLOW_H

#include "Kernel/SubTask.h"

#include <functional>
#include <mutex>
#include <assert.h>

class SeriesWork;
class ParallelWork;

using series_callback_t = std::function<void (const SeriesWork *)>;
using parallel_callback_t = std::function<void (const ParallelWork *)>;

class Workflow
{
public:
	static SeriesWork *
	create_series_work(SubTask *first, series_callback_t callback);

	static void
	start_series_work(SubTask *first, series_callback_t callback);

	static ParallelWork *
	create_parallel_work(parallel_callback_t callback);

	static ParallelWork *
	create_parallel_work(SeriesWork *const all_series[], size_t n,
						 parallel_callback_t callback);

	static void
	start_parallel_work(SeriesWork *const all_series[], size_t n,
						parallel_callback_t callback);

public:
	static SeriesWork *
	create_series_work(SubTask *first, SubTask *last,
					   series_callback_t callback);

	static void
	start_series_work(SubTask *first, SubTask *last,
					  series_callback_t callback);
};

class SeriesWork
{
public:
    void start()
    {
            assert(!this->in_parallel);
            this->first->dispatch();
    }

    //todo: the note of workflow
    void dismiss()
    {
            assert(!this->in_parallel);
            this->dismiss_recursive();
    }

public:
    void push_back(SubTask *task);
    void push_front(SubTask *task);

public:
    void *get_context() const {return this->context;}
    void set_context(void *context) { this->context = context;}

public:
    // todo: translate the note of workflow
    virtual void cancle() {this->canceled = true;}
    bool is_canceled() const {return this->canceled;}
    bool is_finished() const {return !this->first;}

    void set_callback(series_callback_t callback)
    {
            this->callback = std::move(callback);
    }

public:
    SubTask *pop();

    void set_last_task(SubTask *last)
    {
            last->set_pointer(this);
            this->last = last;
    }

    void unset_last_task() { this->last = NULL; }


protected:
    SubTask *get_last_task() const {return this->last;}
    void set_int_parallel() {this->in_parallel = true;}

    void dismiss_recursive();
protected:
    void *context;
    series_callback_t  callback;

private:
    SubTask *first;
    SubTask *last;
    SubTask **queue;
    int queue_size;
    int front;
    int back;
    bool in_parallel;
    bool canceled;
    std::mutex mutex;

    SubTask *pop_task();
    void expand_queue();

protected:
    SeriesWork(SubTask *first,series_callback_t && callback);
    virtual  ~SeriesWork() { delete [] this->queue; }
    friend class ParallelWork;
    friend class Workflow;
};

static inline SeriesWork *series_of(const SubTask *task)
{
	return (SeriesWork *)task->get_pointer();
}

static inline SeriesWork& operator *(const SubTask& task)
{
	return *series_of(&task);
}

static inline SeriesWork& operator << (SeriesWork& series, SubTask *task)
{
	series.push_back(task);
	return series;
}

inline SeriesWork *
Workflow::create_series_work(SubTask *first, series_callback_t callback)
{
	return new SeriesWork(first, std::move(callback));
}

inline void
Workflow::start_series_work(SubTask *first, series_callback_t callback)
{
	new SeriesWork(first, std::move(callback));
	first->dispatch();
}

inline SeriesWork *
Workflow::create_series_work(SubTask *first, SubTask *last,
							 series_callback_t callback)
{
	SeriesWork *series = new SeriesWork(first, std::move(callback));
	series->set_last_task(last);
	return series;
}

inline void
Workflow::start_series_work(SubTask *first, SubTask *last,
							series_callback_t callback)
{
	SeriesWork *series = new SeriesWork(first, std::move(callback));
	series->set_last_task(last);
	first->dispatch();
}

class ParallelWork : public ParallelTask
{
public:
    void start()
    {
            assert(!series_of(this));
    }

    void dismiss()
    {
            assert(!series_of(this));
            delete this;
    }

    void add_series(SeriesWork *series);

    void *get_context() const { return this->context; }
    void set_context(void *context) { this->context = context; }
protected:
    void *context;
    parallel_callback_t callback;
private:
    size_t buf_size;
    SeriesWork **all_series;
};
#endif //SU_WORKFLOW_H
