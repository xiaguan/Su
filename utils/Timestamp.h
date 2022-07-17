//
// Created by root on 22-6-7.
//

#ifndef SU_TIMESTAMP_H
#define SU_TIMESTAMP_H

#include <chrono>
#include <cstring>
#include <iostream>

class Timestamp {
public:
    static constexpr int DATE_SIZE = 12;
    static constexpr int BASE_TIME_SIZE = 10;
    static constexpr int MICROSECOND_SIZE = 8;
    static constexpr int SIZE = 26;
    Timestamp()
            :m_sys_time{std::chrono::system_clock::now()},
             m_date{std::chrono::floor<std::chrono::days>(m_sys_time)}
    {
        last_update = m_sys_time;
        fmt_time(fmt_date(m_buf));
    }

    int year() const noexcept { return static_cast<int>(m_date.year());}
    unsigned month() const noexcept {return static_cast<unsigned>(m_date.month());}
    unsigned day() const noexcept {return static_cast<unsigned>(m_date.day());}

    long hour() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::hours>(m_sys_time.time_since_epoch()).count() % 24;
    }

    long minute() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::minutes>(m_sys_time.time_since_epoch()).count() % 60;
    }

    long second() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::seconds>(m_sys_time.time_since_epoch()).count() % 60;
    }

    long microsecond() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(m_sys_time.time_since_epoch()).count() % 1000000;
    }

    char* fmt_date(char * buf) const
    {
        int w = snprintf(buf,DATE_SIZE,DATE_FMT,year(),month(),day());
        return buf + w;
    }

    char * fmt_time(char * buf) const
    {
        int w = snprintf(buf,BASE_TIME_SIZE,BASE_TIME_FMT,hour(),minute(),second());
        return buf + w;
    }

    char * fmt_mirosecond(char * buf) const
    {
        int w = snprintf(buf,MICROSECOND_SIZE,MICRO_SEC_FMT,microsecond());
        return buf + w;
    }

    char* fmt_all(char * buf)
    {
        fresh_time();
        memcpy(buf,m_buf,20);
        buf += 20;
        buf += snprintf(buf,MICROSECOND_SIZE,MICRO_SEC_FMT,microsecond());
        return buf;
    }

    std::string to_string()
    {
        char buf[40];
        fmt_all(buf);
        return buf;
    }

    void fresh_time() {
        m_sys_time = std::chrono::system_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(m_sys_time - last_update).count() >= 1)
        {
            last_update = m_sys_time;
            m_date =  std::chrono::floor<std::chrono::days>(m_sys_time);
            fmt_time(fmt_date(m_buf));
        }
    }
private:
    static constexpr char BUF_SIZE = 40;
    static constexpr char DATE_FMT[] = "%04d-%02u-%02u ";
    static constexpr char BASE_TIME_FMT[] = "%.02ld:%.02ld:%.02ld ";
    static constexpr char MICRO_SEC_FMT[] = "%.06ld";

    char m_buf[40];
    std::chrono::time_point<std::chrono::system_clock> m_sys_time;
    std::chrono::time_point<std::chrono::system_clock> last_update;
    std::chrono::year_month_day m_date;
};


#endif //SU_TIMESTAMP_H
