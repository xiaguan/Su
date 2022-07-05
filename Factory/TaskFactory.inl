#include "Task.h"
#include "Workflow.h"
#include "TaskFactory.h"

#include "Manager/WFGlobal.h"


template <class INPUT,class OUTPUT>
class __ThreadTask: public WFThreadTask<INPUT,OUTPUT>
{
public:
    __ThreadTask(ExecQueue *queue,Executor *executor,
                 std::function<void (INPUT *,OUTPUT *)>&& routine,
                 std::function<void (WFThreadTask<INPUT,OUTPUT>*)>&& callback)
                 : WFThreadTask<INPUT, OUTPUT>(queue,executor,std::move(callback))
                         ,routine(routine)
    {

    }

protected:
    std::function<void (INPUT *, OUTPUT *)> routine;

    void execute() override
    {
        this->routine(&this->input,&this->output);
    }
};

template<class INPUT, class OUTPUT>
WFThreadTask<INPUT,OUTPUT>
*WFThreadTaskFactory<INPUT, OUTPUT>::create_trhead_task(ExecQueue * queue,
                                                        std::function<void(INPUT *,OUTPUT *)> routine,
                                                        std::function<void(TaskType *)> callback){
    return new __ThreadTask(queue,SettingManager::get_compute_executor(),routine,callback);
}

template<class INPUT, class OUTPUT>
WFThreadTask<INPUT,OUTPUT>
*WFThreadTaskFactory<INPUT, OUTPUT>::create_thread_task(const std::string &queue_name,
                                                        std::function<void(INPUT *,OUTPUT *)> routine,
                                                        std::function<void(TaskType * )> callback) {
    return new __ThreadTask(SettingManager::get_exec_queue(queue_name),SettingManager::get_compute_executor(),
                           std::move(routine),std::move(callback));
}
