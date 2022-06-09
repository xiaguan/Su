//
// Created by susun on 2022/6/2.
//

#include "Buffer.h"

#include <cstring>

FileBuffer::FileBuffer(std::string file_name, int buf_size)
: Buffer(buf_size)
{
    out.open(file_name,std::ios::out);

    // file open failed
    if(!out.is_open()){
        std::cerr <<" FileBuffer open file failed "<<file_name << '\n';
    }
}

FileBuffer::~FileBuffer() {
    output();
    delete [] data;
}

void FileBuffer::output() {
    out.write(data,idx);
    idx = 0;
    cur = data;
}

void Buffer::append(const char *buf, size_t size) {

    memcpy(cur,buf,size);
    addLen(size);
}


StreamBuffer::self &StreamBuffer::operator<<(const void * val) {
    static char digits[17] = "0123456789abcdef" ;

    if(leftSize() < kMaxNumericSize) output();

    char * buf = getCur();
    buf[0] = '0';
    buf[1]= 'x';
    auto * to_hex = (unsigned char * )&val;
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

StreamBuffer::self &StreamBuffer::operator<<(double val) {
    if(leftSize() < kMaxNumericSize) output();
    int len = snprintf(getCur(),kMaxNumericSize,"%f",val);
    addLen(len);
    return *this;
}
