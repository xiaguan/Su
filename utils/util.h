//
// Created by susun on 2022/5/17.
//

#ifndef SU_LIB_UTILS
#define SU_LIB_UTILS

#include <memory>
#include <thread>
#include <sstream>

namespace su{
    template <typename T,class X = void,int N = 0>
    class Singleton{
    public:
        static T * GetInstance(){
            static T v;
            return &v;
        }
    };

    template <typename T,class X = void,int N = 0>
    class SingletonPtr{
    public:
        static std::shared_ptr<T> GetInstance(){
            static std::shared_ptr<T> v(new T);
            return v;
        }
    };

    inline unsigned long long getThreadId(){

        unsigned long long result = 0;
        std::ostringstream osr;
        osr << std::this_thread::get_id();
        result = std::stoull(osr.str());
        return result;
    }

    class TimeCounter{
    public:
        typedef std::chrono::time_point<std::chrono::high_resolution_clock,
        std::chrono::high_resolution_clock::duration> m_timepoint;
        TimeCounter(const std::string name = "TimeCounter "):couter_name(std::move(name)){
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
}

#endif // SU_LIB_UTILS