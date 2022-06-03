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
    idx += size;
}


