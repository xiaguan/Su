//
// Created by susun on 2022/7/5.
//

#include "WFGlobal.h"

#include <unordered_map>


class __Global{
public:
    static __Global* get_instance()
    {
        static __Global global;
        return &global;
    }

private:
    __Global();

    std::unordered_map<std::string,const char *> static_scheme_port_;
    std::unordered_map<std::string,std::string> user_scheme_port_;

  // todo : the mutex

};

__Global::__Global()
{
    static_scheme_port_["dns"] = "53";
    static_scheme_port_["Dns"] = "53";
    static_scheme_port_["DNS"] = "53";

    static_scheme_port_["dnss"] = "853";
    static_scheme_port_["Dnss"] = "853";
    static_scheme_port_["DNSs"] = "853";
    static_scheme_port_["DNSS"] = "853";

    static_scheme_port_["http"] = "80";
    static_scheme_port_["Http"] = "80";
    static_scheme_port_["HTTP"] = "80";

    static_scheme_port_["https"] = "443";
    static_scheme_port_["Https"] = "443";
    static_scheme_port_["HTTPs"] = "443";
    static_scheme_port_["HTTPS"] = "443";

    static_scheme_port_["redis"] = "6379";
    static_scheme_port_["Redis"] = "6379";
    static_scheme_port_["REDIS"] = "6379";

    static_scheme_port_["rediss"] = "6379";
    static_scheme_port_["Rediss"] = "6379";
    static_scheme_port_["REDISs"] = "6379";
    static_scheme_port_["REDISS"] = "6379";

    static_scheme_port_["mysql"] = "3306";
    static_scheme_port_["Mysql"] = "3306";
    static_scheme_port_["MySql"] = "3306";
    static_scheme_port_["MySQL"] = "3306";
    static_scheme_port_["MYSQL"] = "3306";

    static_scheme_port_["mysqls"] = "3306";
    static_scheme_port_["Mysqls"] = "3306";
    static_scheme_port_["MySqls"] = "3306";
    static_scheme_port_["MySQLs"] = "3306";
    static_scheme_port_["MYSQLs"] = "3306";
    static_scheme_port_["MYSQLS"] = "3306";

    static_scheme_port_["kafka"] = "9092";
    static_scheme_port_["Kafka"] = "9092";
    static_scheme_port_["KAFKA"] = "9092";
}

class __ExecManager
{
protected:
    using ExecQueueMap = std::unordered_map<std::string,ExecQueue *>;

public:
    static __ExecManager * get_instance()
    {
        static __ExecManager execManager;
        return &execManager;
    }

    Executor *get_compute_executor() {return &m_executor;}

    ExecQueue * get_exec_queue(const std::string& queue_name)
    {
        ExecQueue * result = nullptr;

        pthread_rwlock_rdlock(&rwlock);
    // we want const_iterator
        ExecQueueMap::const_iterator it = m_queue_map.find(queue_name);
        if(it != m_queue_map.cend())
        result = it->second;

        pthread_rwlock_unlock(&rwlock);
        if(result)
            return result;

        pthread_rwlock_wrlock(&rwlock);
        it = m_queue_map.find(queue_name);
        if(it == m_queue_map.cend())
        {
            result = new ExecQueue();
            if(result->init() >= 0)
            {
                m_queue_map.emplace(queue_name,result);
            }
            else
            {
                delete result;
                return nullptr;
            }
        }
        else
        {
        result = it->second;
        }

        pthread_rwlock_unlock(&rwlock);
        return result;
    }

private:
    __ExecManager();
    Executor m_executor;
    pthread_rwlock_t rwlock;
    ExecQueueMap m_queue_map;
};

__ExecManager::__ExecManager()
:rwlock(PTHREAD_RWLOCK_INITIALIZER)
{
    int ret = m_executor.init(SettingManager::get_settings()->compute_threads);
    if(ret < 0 )
        abort();
}

ExecQueue *SettingManager::get_exec_queue(const std::string &queue_name) {
    return __ExecManager::get_instance()->get_exec_queue(queue_name);
}

Executor *SettingManager::get_compute_executor() {
    return __ExecManager::get_instance()->get_compute_executor();
}
