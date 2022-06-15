//
// Created by root on 22-6-14.
//

#ifndef SU_FILEINFO_H
#define SU_FILEINFO_H

#include <string>
#include <filesystem>
#include <unordered_map>

#include <iostream>

constexpr int ENCODER_READ_SIZE = 256*1024;
constexpr int ENCODER_WRITE_SIZE = 2*ENCODER_READ_SIZE + 20;




inline void convert(const std::string & s, int& idx, int& bias , char * buf)
{
    for(char i : s)
    {
        if(bias == - 1){
            ++idx;
            bias = 7;
        }
        if(i == '1')
        {
            buf[idx] = ((1 << bias) | buf[idx]);
        }
        bias--;
    }
}

// ! : only for zipper
std::ifstream open_read_file(const std::string & name);

template <typename T>
std::string get_two_string(const T& t){
    int sz= sizeof(T);
    char * ptr = (char*)&t;
    std::string result;
    for(int i = 0;i<sz;i++){
        for(int j =  7 ;j>=0;j--){
            result.push_back((ptr[i] >> j ) & 1 ? '1':'0');
        }
    }
    return result;
}

struct MapElement{
    unsigned char length;  // how long the value is
    char buf[2];   // value
    char key;     // key
    ~MapElement() = default;
};

struct FileInfo{
    short name_sz;
    char * file_name{nullptr};
    unsigned long long file_size;
    short map_size;
    MapElement * map{nullptr};

    void format_name(const std::string & filename);

    void format_map(const std::unordered_map<char,std::string> & table);

    void output(std::ofstream & out);
    void input(std::ifstream & in);
    std::unordered_map<std::string,char> getMap();
    unsigned long long check_map_hash();

    ~FileInfo()
    {
        delete [] file_name;
        free(map);
    }
};


#endif //SU_FILEINFO_H
