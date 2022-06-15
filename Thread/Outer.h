//
// Created by root on 22-6-15.
//

#ifndef SU_OUTER_H
#define SU_OUTER_H

#include <condition_variable>
#include <iostream>
#include <mutex>

class Outer {
public:
    Outer(std::ofstream control_out_stream) : idx(0),out(std::move(control_out_stream)){}

    void output(char * buf, size_t n,int order){
            m_mut.lock();
            if(idx == order) {
                    push_out(buf,n);
                    m_mut.unlock();
                    return;
            }
            m_mut.unlock();
            std::unique_lock<std::mutex> lk(m_mut);
            wait_pre_idx.wait(lk,[this,order]{return idx == order;});
            push_out(buf,n);
    }

    unsigned long long size() const noexcept { return out_size;}
private:
    void push_out(char * buf,size_t n)
    {
            ++idx;
            out.write(buf,n);
            out << std::flush;
            out_size += n;
            wait_pre_idx.notify_all();
    }
    std::ofstream  out;
    int idx;
    std::mutex m_mut;
    std::condition_variable wait_pre_idx;
    unsigned long long out_size{0};
};


#endif //SU_OUTER_H
