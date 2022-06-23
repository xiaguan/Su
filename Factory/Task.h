//
// Created by root on 22-6-21.
//

#ifndef SU_TASK_H
#define SU_TASK_H

#include "Kernel/Executor.h"
#include "Kernel/ExecRequest.h"

#include "Workflow.h"

// todo : finish the enum
enum
{
    TASK_STATE_UNDFINED = -1
};

template <class INPUT,class OUTPUT>
class WFThreadTask : public ExecRequest
{
public:
    WFThreadTask(ExecQueue *queue,Executor *executor,
                 std::function<void (WFThreadTask<INPUT,OUTPUT> *)> cb)
                 : ExecRequest(queue,executor),callback(std::move(cb))
    {
            this->user_data = nullptr;
            this->state = TASK_STATE_UNDFINED;
            this->error = 0;
    }
    void start();  // todo:workflow
    void dismiss();

    INPUT *get_input() { return &this->input; }
    OUTPUT *get_output() { return &this->output; }

    int get_state() const { return this->state; }
    int get_error() const { return this->error; }

    void set_callback(std::function<void (WFThreadTask<INPUT,OUTPUT> *) > cb)
    {
            this->callback = std::move(cb);
    }

    virtual SubTask *done() override
    {
            SeriesWork *series = series_of(this);

            if(this->callback)
                    this->callback(this);

            delete this;
            return series->pop();
    }

public:
    void *user_data{nullptr};

protected:
    INPUT input;
    OUTPUT output;
    std::function<void (WFThreadTask<INPUT,OUTPUT> *) > callback;
    virtual ~WFThreadTask() { }
};

#endif //SU_TASK_H
