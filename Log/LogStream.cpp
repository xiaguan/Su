//
// Created by susun on 2022/6/2.
//

#include "LogStream.h"


LogStream& LogStream::operator<<(double val) {
    if(buffer.leftSize() < kMaxNumericSize) buffer.output();
    int len = snprintf(buffer.getCur(),kMaxNumericSize,"%f",val);
    buffer.addLen(len);
    return *this;
}

LogStream& LogStream::operator<<(const void * val) {

    static char digits[17] = "0123456789abcdef" ;

    if(buffer.leftSize() < kMaxNumericSize) buffer.output();

    char * buf = buffer.getCur();
    buf[0] = '0';
    buf[1]= 'x';
    unsigned char * to_hex = (unsigned char * )&val;
    int idx = 13;
    for(int i = 0;i<6;i++){
        buf[idx] = digits[(*to_hex) % 16];
        --idx;
        buf[idx] =digits[(*to_hex) / 16];
        --idx;
        ++to_hex;
    }
    return *this;
}