#include <iostream>
#include <cstring>

#include "Log/LogStream.h"

using namespace std;

const int test_num = 1e7;

#include <chrono>
class Timer{
public:
    explicit Timer(std::string  name ="Timer"):m_name(std::move(name)){}
    void start() noexcept {
        begin = std::chrono::high_resolution_clock::now();
    }
    void end(){
        auto end = std::chrono::high_resolution_clock::now();
        auto result = std::chrono::duration<double,std::ratio<1,1000>>(end-begin);
        std::cout <<m_name<<" live for "<<result.count() <<" ms"<<std::endl;
    }
private:
    std::string m_name;
    decltype(std::chrono::high_resolution_clock::now()) begin;
};

template <typename T>
void test(){
    LogStream ss("test.txt");
    Timer timer;
    timer.start();
    for(size_t i = 0;i<test_num;++i){
        ss <<(T)(i);
        ss.reset();
    }
    timer.end();
}

int main(){
        LogStream ss("default.txt");
        ss <<'a'<<"\t"<<"hh";
}