//
// Created by susun on 2022/6/2.
//

#ifndef SU_BUFFER_H
#define SU_BUFFER_H

#include <fstream>
#include <iostream>

#include <cstring>

class Buffer {
public:
    Buffer(int size):bufSize(size),idx(0){
        data = new char[bufSize];
        cur = data;
    }

    [[nodiscard]] int leftSize() const {return bufSize - idx;}

    void addLen(int len) { idx += len; cur += len;}
    char *  getCur() const {return cur;}

    void append(const char * buf, size_t size);

    // out put to anywhere file ,std::cout , even the net
    // no virtual is ok with UNIX fd
    virtual void output() = 0;

    void reset(){ cur = data;idx = 0;}

    int size() const noexcept { return idx +1;}

    virtual ~Buffer() { delete [] data;}
protected:
    char * data;
    int bufSize;
    int idx;
    char * cur;
};

namespace detail{
    // better itoa
    static constexpr unsigned b2 = 100;
    static constexpr unsigned b3 = 1000;
    static constexpr unsigned  b4 = 10000;

    template <std::integral T>
    static  unsigned gcc_get_int_len(T val){
        static constexpr unsigned base = 10;
        unsigned int n = 1;
        while(true){
            if(val < base ) return n;
            if(val < b2) return n+1;
            if(val < b3) return n+2;
            if(val < b4) return n+3;
            val /= b4;
            n += 4;
        }
    }

    template <std::unsigned_integral T>
    static int conver_unsigned(T val,char * buf,int len){
        static constexpr char __digits[201] =
                "0001020304050607080910111213141516171819"
                "2021222324252627282930313233343536373839"
                "4041424344454647484950515253545556575859"
                "6061626364656667686970717273747576777879"
                "8081828384858687888990919293949596979899";

        // we need three param
        // char * first
        // int val
        // unsigned valLen (the len of the val)

        unsigned pos = len -1;

        while(val >= 100){
            auto const num = (val % 100) * 2;  // to find the idx in digits
            val /= 100;
            buf[pos] = __digits[num + 1];
            buf[pos - 1] = __digits[num];
            pos -= 2;
        }

        if(val >= 10){
            auto const num  = val * 2;
            buf[1] = __digits[num+1];
            buf[0] = __digits[num];
        }else{
            buf[0] = '0' + val;
        }
        return len;
    }

    template <std::signed_integral T>
    static int convert_signed(char * buf,T val){
        // if val is negative then isNeg  = 1; so we skip the firt byte to write
        const bool isNeg = val < 0;
        const unsigned unVal = isNeg ? (unsigned)~val + 1u : val;
        const auto valLen = gcc_get_int_len(unVal);

        buf[0] = '-';

        conver_unsigned(unVal,&buf[isNeg],valLen);

        int result = valLen + isNeg;

        return result;
    }
}

constexpr int kMaxNumericSize = 48;

class StreamBuffer : public Buffer{
public:
    typedef StreamBuffer self;

    explicit StreamBuffer(int buf_size) : Buffer(buf_size) {}
    void output() override = 0;
    ~StreamBuffer() override = default;

    template <std::signed_integral T>
    self& operator<<(T val){
        if(leftSize() >= kMaxNumericSize){
            addLen( detail::convert_signed(getCur(),val));
        }else{
            output();
        }
        return *this;
    }

    template<std::unsigned_integral T>
    self& operator<<(T val){
        if(leftSize() >= kMaxNumericSize){
            int len = detail::gcc_get_int_len(val);
            addLen( detail::conver_unsigned(val,cur,len));
        }else{
            output();
        }
        return *this;
    }

    self& operator<<(const void*);

    self& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    self& operator<<(double);


    self& operator<<(char v)
    {

        append(&v, 1);
        return *this;
    }

    // self& operator<<(signed char);
    // self& operator<<(unsigned char);

    self& operator<<(const char* str)
    {
        if (str)
        {
            append(str, strlen(str));
        }
        else
        {
            append("(null)", 6);
        }
        return *this;
    }


    self& operator<<(const std::string& v)
    {
        append(v.c_str(), v.size());
        return *this;
    }

private:

};


class FileBuffer : public Buffer{
public:
    FileBuffer(std::string file_name,int buf_size);
    void output() override;
    ~FileBuffer() override;
private:
    std::ofstream out;
};





#endif //SU_BUFFER_H
