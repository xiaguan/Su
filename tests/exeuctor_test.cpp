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
    }
    SubTask *done() override
    {
        return nullptr;
    }
    void display() {std::cout << cnt << std::endl;}
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
    for(int i = 0;i<10000;i++) test.dispatch();
    int a;
    std::cin >> a;
    test.display();
}
