//
// Created by susun on 2022/6/28.
//

#ifndef SU_FAST_LOG_H
#define SU_FAST_LOG_H

#include <Log/Buffer.h>

// todo : fast log in unix
class FastLogger : public StreamBuffer
{
public:
    void output() override
    {

    }
private:

};

#endif //SU_FAST_LOG_H
