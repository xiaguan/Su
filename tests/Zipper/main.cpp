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
    std::uniform_int_distribution<char> u(0,255);
    for(int i = 0;i<100000;i++)
        out << "abcdefg !@#$^&%&^*\n";
}

//628 160mb/s
// 2588 40mb/s

int main(){
    preprare();
    su::TimeCounter timeCounter;
    Encoder encoder;
    encoder.zip_file("test.pdf");
    timeCounter.end_and_cout();
    timeCounter.start();
    Decoder decoder;
    decoder.decode("test.pdf.huf");
    timeCounter.end_and_cout();
}
