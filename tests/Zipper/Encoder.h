//
// Created by susun on 2022/6/9.
//

#ifndef SU_ENCODER_H
#define SU_ENCODER_H

#include "FileInfo.h"

#include <unordered_map>
#include <string>
#include <fstream>
#include <filesystem>
#include <map>
#include <vector>

#include <cstring>


class Scanner{
public:
    Scanner(){
        memset(dp,0,sizeof(dp));
    }
    void operator()(const std::string& file_name);
    long long * getData() {return dp;}
private:
    long long dp[256];
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


class Encoder {
public:
    Encoder(){
//        auto logger = SU_LOG_ROOT();
//        logger->setFileAppender("encoder_log.txt");
    }
    void zip_file(const std::string & zip_file_name);
private:
    HuffMan m_tree;
    Scanner scanner{};
    std::unordered_map<char, std::string> zip_table;
    FileInfo file_info;
};


#endif //SU_ENCODER_H
