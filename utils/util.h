//
// Created by susun on 2022/5/17.
//

#ifndef SU_LIB_UTILS
#define SU_LIB_UTILS

#include <memory>

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
}

#endif // SU_LIB_UTILS