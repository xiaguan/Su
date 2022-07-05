//
// Created by susun on 2022/7/5.
//

#ifndef SU_WFGLOBAL_H
#define SU_WFGLOBAL_H

#include "Kernel/Executor.h"

#include <string>

struct GlobalSettings
{
    int compute_threads;
};

static constexpr GlobalSettings DEFTAUL_SETTINGS = {
        .compute_threads = 15
};

class SettingManager{
public:
  static const GlobalSettings* get_settings() {return &m_settings;}
  static Executor *get_compute_executor();
  static ExecQueue *get_exec_queue(const std::string& queue_name);
private:
  static GlobalSettings m_settings;
};

#endif //SU_WFGLOBAL_H
