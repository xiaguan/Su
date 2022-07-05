//
// Created by susun on 2022/7/5.
//

#include <Kernel/Executor.h>
#include <Kernel/ExecRequest.h>

#include <iostream>
#include <atomic>

class Test : public ExecRequest{
public:
    Test(ExecQueue*queue,Executor *executor): ExecRequest(queue,executor){}
    void execute() override
    {
        ++cnt;
        std::cout <<"hello"<<std::endl;
    }
    SubTask *done() override
    {
        return nullptr;
    }
private:
    std::atomic<int> cnt{0};
};

int main()
{
    Executor executor;
    ExecQueue queue;
    executor.init(1);
    queue.init();
    Test test(&queue,&executor);

    int a;
    std::cin >> a;
}
