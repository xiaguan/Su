//
// Created by susun on 2022/6/2.
//

#ifndef SU_BUFFER_H
#define SU_BUFFER_H

#include <fstream>
#include <iostream>


class Buffer {
public:
    Buffer(int size = 1024):bufSize(size),idx(0){
        data = new char[bufSize];
        cur = data;
    }

    [[nodiscard]] int leftSize() const {return bufSize - idx;}

    void addLen(int len) { idx += len; cur += len;}
    char *  getCur() const {return cur;}

    void append(const char * buf, size_t size);

    // out put to anywhere file ,std::cout , even the net
    // no virtual is ok with UNIX fd
    virtual void output() = 0;

    void reset(){ cur = data;idx = 0;}

    virtual ~Buffer() = default;
protected:
    char * data;
    int bufSize;
    int idx;
    char * cur;
};


class FileBuffer : public Buffer{
public:
    FileBuffer(std::string file_name,int buf_size);
    void output() override;
    ~FileBuffer() override;
private:
    std::ofstream out;
};


#endif //SU_BUFFER_H
