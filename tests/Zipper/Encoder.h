//
// Created by susun on 2022/6/9.
//

#ifndef SU_ENCODER_H
#define SU_ENCODER_H

#include <Log/log.h>


#include <unordered_map>
#include <string>
#include <fstream>
#include <filesystem>


#include <cstring>

inline su::log::Logger::ptr logger;

constexpr int ENCODER_READ_SIZE = 256*1024;
constexpr int ENCODER_WRITE_SIZE = 2*ENCODER_READ_SIZE + 20;

// ! : only for zipper
inline std::ifstream open_read_file(const std::string & name)
{
    std::ifstream result(name,std::ios::binary);
    result >> std::noskipws;
    SU_LOG_DEBUG(logger) <<"try to open file "<<name<<" "<<result.is_open();
    return std::move(result);
}

class Scanner{
public:
    Scanner(){
        memset(dp,0,sizeof dp);
    }
    void operator()(const std::string& file_name);
    long long * getData() {return dp;}
private:
    long long dp[300];
    constexpr static int SCAN_BUF_SIZE = 1024*1024;
};

struct Node{
    typedef std::shared_ptr<Node> ptr;
    bool is_value{false};
    long long val; // 权重
    char c ; // 一个字节的值
    Node::ptr left;
    Node::ptr right;
};

class HuffMan{
public:
    void construct(long long * dp);
    std::map<std::string,char> decode;
    std::unordered_map<char,std::string> encode;
private:
    void makeTree(const std::vector<Node::ptr> & nodes);
    void dfs(const Node::ptr& node ,const std::string&  prefix);
    Node::ptr root;
};

struct MapElement{
    unsigned char length;  // how long the value is
    char buf[2];   // value
    char key;     // key
};

struct FileInfo{
    short name_sz;
    char * file_name;
    unsigned long long file_size;
    short map_size;
    MapElement * map;

    void format_name(const std::string & filename)
    {
        name_sz = filename.size();
        file_name = new char[filename.size()+1];
        file_name[name_sz] = 0;
        for(int i = 0;i<filename.size() ;++i) file_name[i] = filename[i];
        file_size = std::filesystem::file_size(file_name);
    }

    void format_map(const std::unordered_map<char,std::string> & table);

    ~FileInfo()
    {
        delete [] file_name;
        //delete [] map;
    }
};

class Encoder {
public:
    Encoder(){
        logger = std::make_shared<su::log::Logger>("zipper");
        logger->setFileAppender("encoder_log.txt");

    }
    void zip_file(const std::string & zip_file_name);
private:
    HuffMan m_tree;
    Scanner scanner;
    std::unordered_map<char, std::string> zip_table;
    FileInfo file_info;
};


#endif //SU_ENCODER_H
