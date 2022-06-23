//
// Created by root on 22-6-20.
//

#ifndef SU_EXECREQUEST_H
#define SU_EXECREQUEST_H

#include "SubTask.h"
#include "Executor.h"

/*
 * the class's main responsibility is to wrapper the executor->request(queue,executor)
 * and make a ExecRequest to be a task ,because ExecSession doesn't inherit the SubTask
 */
class ExecRequest : public SubTask , public ExecSession
{
public:
    ExecRequest(ExecQueue *queue,Executor *executor)
    :queue(queue),executor(executor)
    {

    }

    ExecQueue *get_request_queue() const { return queue; }
    void set_request_queue(ExecQueue *queue) { this->queue = queue; }

    virtual void dispatch() override
    {
            if(this->executor->request(this,this->queue) < 0)
                    this->handle(ES_STATE_ERROR,errno);
    }

protected:
    int state;
    int error;

protected:
    ExecQueue *queue;  // todo: there is still Execsession's queue?
    Executor *executor;

protected:
    virtual void handle(int state,int error) override
    {
            this->state = state;
            this->error = error;
            this->subtask_done();
    }
};

#endif //SU_EXECREQUEST_H
