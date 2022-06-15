//
// Created by susun on 2022/6/9.
//

#include "Encoder.h"
#include <utils/util.h>
#include "Decoder.h"

#include <random>

void preprare()
{
    std::ofstream out("test.txt");
    std::default_random_engine  engine;
    std::uniform_int_distribution<char> u(70,127);
    for(int i = 0;i<1e5;i++)
        out << "something is just like this QWEUOI ZO !@#$%^&*()_+{}|:"<<'\n';
}


int main(){
   preprare();
    su::TimeCounter timeCounter;
    Encoder encoder;
    encoder.zip_file("test.txt");
    timeCounter.end_and_cout();
    Decoder decoder;
    std::string test="test";
    std::cout << std::string(test.begin(),test.begin() + 1)<<std::endl;
    decoder.decode("test.txt.huf");
    timeCounter.end_and_cout();
}
