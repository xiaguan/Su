//
// Created by susun on 2022/5/17.
//

#ifndef SU_LIB_UTILS
#define SU_LIB_UTILS

template <typename T>
class SingeTon{
public:
    T& getInstance(){
        static T t;
        return t;
    }
};

#endif // SU_LIB_UTILS