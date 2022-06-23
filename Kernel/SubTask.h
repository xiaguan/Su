//
// Created by root on 22-6-19.
//

#ifndef SU_SUBTASK_H
#define SU_SUBTASK_H

#include <stddef.h>

class ParallelTask;

/*
 * we see the SubTask mostly by multi-inherit
 * The SubTask describe the most fundamental task in our lib
 * // todo: explain the SubTask and ParallelTask
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
