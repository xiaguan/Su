//
// Created by susun on 2022/6/30.
//

#ifndef SU_TASKFACTORY_H
#define SU_TASKFACTORY_H

#include "Task.h"

template <class INPUT, class OUTPUT>
class WFThreadTaskFactory
{
private:
    using TaskType = WFThreadTask<INPUT,OUTPUT>;
    //todo: multi

public:
    static TaskType *create_trhead_task(ExecQueue * queue,
                                        std::function<void (INPUT *,OUTPUT *)> routine,
                                        std::function<void (TaskType *)> callback);

    static TaskType *create_thread_task(const std::string & queue_name,
                                        std::function<void (INPUT *,OUTPUT *)> routine,
                                        std::function<void (TaskType *)> callback);
};

#include "TaskFactory.inl"


#endif //SU_TASKFACTORY_H
