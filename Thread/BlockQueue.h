//
// Created by susun on 2022/6/12.
//

#ifndef SU_BLOCKQUEUE_H
#define SU_BLOCKQUEUE_H

#include <condition_variable>
#include <queue>
#include <mutex>



template <typename T>
class BlockQueue
{
public:
    BlockQueue(int max_size) : max_queue_size(max_size) {}
    void push(T new_val) {
        std::unique_lock<std::mutex> lk(m_mut);
        is_full.wait(lk, [this]() {return m_queue.size() <= max_queue_size; });
        m_queue.push(std::move(new_val));
        is_empty.notify_one();
    }
    T get_front() {
        std::unique_lock<std::mutex> lk(m_mut);
        is_empty.wait(lk, [this]() { return !this->m_queue.empty(); });
        auto t = std::move(m_queue.front());
        m_queue.pop();
        is_full.notify_one();
        return t;
    }
    size_t size() const {
        std::lock_guard<std::mutex> guard(m_mut);
        return m_queue.size();
    }

private:
    std::queue<T> m_queue;
    mutable std::mutex m_mut;

    std::condition_variable is_empty;
    std::condition_variable is_full;
    int max_queue_size;
};

#endif //SU_BLOCKQUEUE_H
