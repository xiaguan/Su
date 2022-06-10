//
// Created by susun on 2022/6/12.
//

#ifndef SU_THREADPOOL_H
#define SU_THREADPOOL_H

#include "BlockQueue.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>

#include <functional>


class ThreadPool {
public:
    using Task = std::function<void()>;

private:
    std::vector<std::thread> workers;
};


#endif //SU_THREADPOOL_H
