//
// Created by susun on 2022/6/28.
//

#ifndef SU_FAST_LOG_H
#define SU_FAST_LOG_H

#include <Log/Buffer.h>

/*
 * 异步高性能日志的设计，首先它的功能必须足够简单
 * 比如这样： log::debug << "hello world<<;
 * 我们可以为日志申请一个ExecQueue,名字就叫log
 * 然后不断将log任务发布出去
 *
 * log任务该怎么发布？
 */

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
