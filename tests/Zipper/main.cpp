//
// Created by susun on 2022/6/9.
//

#include "Encoder.h"
#include <utils/util.h>

#include <random>

void preprare()
{
    std::ofstream out("test.txt");
    std::default_random_engine  engine;
    std::uniform_int_distribution<char> u(-128,127);
    for(int i = 0;i<1e7;i++)
        out << u(engine);
}

int main(){
    preprare();
    su::TimeCounter timeCounter;
    Encoder encoder;
    encoder.zip_file("test.txt");
    timeCounter.end_and_cout();
}
