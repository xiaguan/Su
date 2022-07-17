#include <iostream>
#include <chrono>
#include <ctime>
#include <sys/time.h>
#include <inttypes.h>
#include <cstring>
#include <thread>
using namespace std;

using namespace chrono;

const int TEST_NUMS = 1e7;

class Timestamp {
public:
    static constexpr int DATE_SIZE = 12;
    static constexpr int BASE_TIME_SIZE = 10;
    static constexpr int MICROSECOND_SIZE = 8;
    static constexpr int SIZE = 26;
    using time_type = std::chrono::hh_mm_ss<std::chrono::microseconds>;
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

class TimeCounter{
public:
    typedef std::chrono::time_point<std::chrono::high_resolution_clock,
            std::chrono::high_resolution_clock::duration> m_timepoint;
    TimeCounter(const std::string name = "TimeCounter "):couter_name(std::move(name))
    {
        begin = std::chrono::high_resolution_clock::now();
    }

    void start()
    {
        begin = std::chrono::high_resolution_clock::now();
    }

    void end_and_cout()
    {
        m_timepoint end = std::chrono::high_resolution_clock::now();
        std::cout << couter_name <<" count "<<(end-begin).count()/1000000 << " ms \n";
    }
private:
    m_timepoint begin;
    std::string couter_name;
};

namespace muduo
{
    class Timestamp
    {
    public:
        ///
        /// Constucts an invalid Timestamp.
        ///
        Timestamp()
                : microSecondsSinceEpoch_(0)
        {
        }

        ///
        /// Constucts a Timestamp at specific time
        ///
        /// @param microSecondsSinceEpoch
        explicit Timestamp(int64_t microSecondsSinceEpochArg)
                : microSecondsSinceEpoch_(microSecondsSinceEpochArg)
        {
        }

        void swap(Timestamp& that)
        {
            std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
        }

        // default copy/assignment/dtor are Okay

        string toString() const;
        string toFormattedString(bool showMicroseconds = true) const;

        bool valid() const { return microSecondsSinceEpoch_ > 0; }

        // for internal usage.
        int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
        time_t secondsSinceEpoch() const
        { return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond); }

        ///
        /// Get time of now.
        ///
        static Timestamp now();
        static Timestamp invalid()
        {
            return Timestamp();
        }

        static Timestamp fromUnixTime(time_t t)
        {
            return fromUnixTime(t, 0);
        }

        static Timestamp fromUnixTime(time_t t, int microseconds)
        {
            return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
        }

        static const int kMicroSecondsPerSecond = 1000 * 1000;

    private:
        int64_t microSecondsSinceEpoch_;
    };

    inline bool operator<(Timestamp lhs, Timestamp rhs)
    {
        return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
    }

    inline bool operator==(Timestamp lhs, Timestamp rhs)
    {
        return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
    }

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microsecond
/// resolution for next 100 years.
    inline double timeDifference(Timestamp high, Timestamp low)
    {
        int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
        return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
    }

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
///
    inline Timestamp addTime(Timestamp timestamp, double seconds)
    {
        int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
        return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
    }

    static_assert(sizeof(Timestamp) == sizeof(int64_t),
                  "Timestamp should be same size as int64_t");

    string Timestamp::toString() const
    {
        char buf[32] = {0};
        int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
        int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
        snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
        return buf;
    }

    string Timestamp::toFormattedString(bool showMicroseconds) const
    {
        char buf[64] = {0};
        time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
        struct tm tm_time;
        gmtime_r(&seconds, &tm_time);

        if (showMicroseconds)
        {
            int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
            snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                     tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                     tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                     microseconds);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                     tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                     tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        }
        return buf;
    }

    Timestamp Timestamp::now()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t seconds = tv.tv_sec;
        return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
    }
}
char muduo_buf[60];
void muduo_cast(char * buf,muduo::Timestamp &time_)
{
    static time_t t_lastSecond;
    using muduo::Timestamp;
    int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
    if (seconds != t_lastSecond)
    {
        t_lastSecond = seconds;
        struct tm tm_time;
        ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime

        int len = snprintf(muduo_buf, 60, "%4d-%02d-%02d %02d:%02d:%02d",
                           tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                           tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    int result = snprintf(muduo_buf+19,40, " %.06d",microseconds);
    memcpy(buf,muduo_buf,60);
}

void test_mine()
{
    char buf[40];
    Timestamp timestamp;
    for(int i = 0;i<10;i++)
    {
        timestamp.fmt_all(buf);
        std::cout << buf << std::endl;
        std::this_thread::sleep_for(200ms);
    }
    std::cout << timestamp.to_string() << " size : "<<timestamp.to_string().size() <<std::endl;
    std::cout <<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<std::endl;
    TimeCounter timeCounter;
    for(int i = 0;i<TEST_NUMS;i++)
    {
        timestamp.fmt_all(buf);
    }
    timeCounter.end_and_cout();
}

void test_muduo(){
    char buf[60];
    for(int i = 0;i<10;i++)
    {
        muduo::Timestamp timestamp(muduo::Timestamp::now());
        muduo_cast(buf,timestamp);
        std::cout << buf << std::endl;
        std::this_thread::sleep_for(200ms);
    }
    TimeCounter timeCounter;
    for(int i = 0;i<TEST_NUMS;i++)
    {
        muduo::Timestamp timestamp(muduo::Timestamp::now());
        muduo_cast(buf,timestamp);
    }
    timeCounter.end_and_cout();
}


int main(){
    test_mine();
    test_muduo();
}