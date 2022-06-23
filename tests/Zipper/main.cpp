//
// Created by susun on 2022/6/9.
//

#include "Encoder.h"
#include <utils/util.h>
#include "Decoder.h"

#include <random>

void preprare()
{
    std::ofstream out("test.txt",std::ios::binary);
    std::default_random_engine  engine;
    std::uniform_int_distribution<char> u(70,127);
    for(int i = 0;i<100*1024*1024;i++)
        out << u(engine);
}

//628 160mb/s
// 2588 40mb/s

int main(){
       // preprare();
    su::TimeCounter timeCounter;
    Decoder decoder;
    decoder.decode("test.txt.huf");
    timeCounter.end_and_cout();
}
