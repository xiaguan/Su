//
// Created by root on 22-6-19.
//

#ifndef SU_SUBTASK_H
#define SU_SUBTASK_H

#include <stddef.h>

class ParallelTask;

/*
 * 大部分情况下我们看到的SubTask都是多继承，换句话说
 * 它是接口基类，它规定了workflow中最基本的任务描述
 * 关于它有这么几点需要注意：
 * 1. dispatch() 是纯虚函数，子类需要覆盖这个接口// todo: when call dispatch()
 * 2. done() 也是纯虚函数，子类需要覆盖//todo:
 * 2. void * pointer 指针，我感觉void *,就是Cstyle的泛型
 * // todo : task and paralleltask
 *
 */
class SubTask {
public:
    virtual void dispatch() = 0;

    ParallelTask *get_parent_task() const {return this->parent;}
    void *get_pointer() const {return pointer;}
    void set_pointer(void *pointer) {this->pointer = pointer;}

    SubTask():parent(nullptr),entry(nullptr),pointer(nullptr) { }

    virtual ~SubTask() { }
    friend class ParallelTask;
protected:
    void subtask_done();
private:
    ParallelTask *parent;
    SubTask **entry;
    void *pointer;
    virtual SubTask *done() = 0;
};

class ParallelTask : public SubTask
{
public:
    void dispatch() override;

    ParallelTask(SubTask **subtasks,size_t n)
    :subtasks(subtasks),subtasks_nr(n)
    {

    }

    SubTask **get_subtasks(size_t *n ) const
    {
            *n = this->subtasks_nr;
            return this->subtasks;
    }

    void set_subtasks(SubTask **subtasks,size_t n)
    {
            this->subtasks = subtasks;
            this->subtasks_nr = n;
    }

protected:
    SubTask **subtasks;
    size_t subtasks_nr;

private:
    size_t nleft;
    friend class SubTask;
};


#endif //SU_SUBTASK_H
