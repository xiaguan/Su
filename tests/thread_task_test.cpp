//
// Created by susun on 2022/7/5.
//

#include <Factory/TaskFactory.h>
#include <iostream>
#include <vector>
#include <mutex>

using PrimeTask = std::function<void(WFThreadTask<int,int> *task)>;

void routine(int * in, int * out)
{
    for(int i = 2;i<*in;++i)
    {
        bool flag = false;
        for(int j = 2;j * j<= i;++j)
        {
            if(i % j == 0){
                flag = true;
                break;
            }
        }
        if(!flag) *out++;
    }
}


std::vector<std::pair<int,int>> prim_num;
std::mutex m_mut;
void task_call_back(WFThreadTask<int,int> * task)
{
    std::lock_guard<std::mutex> guard(m_mut);
    int result = *task->get_output();
    int num = *task->get_input();
    prim_num.emplace_back(num,result);
}

int main()
{
    PrimeTask callback(task_call_back);
    auto task = WFThreadTaskFactory<int,int>::create_thread_task("test",std::function<void(int*,int*)>(routine),
                                                                       callback);
}
