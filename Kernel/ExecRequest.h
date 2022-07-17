//
// Created by root on 22-6-20.
//

#ifndef SU_EXECREQUEST_H
#define SU_EXECREQUEST_H

#include "SubTask.h"
#include "Executor.h"
#include "errno.h"

/*
 * 这是kernel的执行体系：Executor...等提供给外界的接口，翻译过来就是执行请求嘛
 * 首先执行请求肯定要有执行者，exeuctor，构造之后就不能改变
 * 然后就是queue,普通任务可以忽略这个东西
 *
 * 既然继承的subtask,就要覆盖两个方法一个市dispatch(),派发任务
 * 剩下一个就是handle函数，处理任务状态的，处理完好调用subtask_done
 * 这就大概是这个类的作用
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
    /*
     * 关于queue这个变量，其实这个变量也存在域父类ExecSession的private里
     * 并不存在重命名的问题，因为private对子类是不可见的
     * 调用dispathc() 的时候这两个队列实际上相等
     * 但考虑到可能ExecRequest可以通过set_request_queue更改队列，所以将其区分
     */
    ExecQueue *queue;
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
