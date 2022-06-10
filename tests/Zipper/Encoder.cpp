//
// Created by susun on 2022/6/9.
//

#include "Encoder.h"

#include <filesystem>
#include <queue>

GET_LOGGER;

template <typename T>
std::string get_two_string(const T& t){
    int sz= sizeof(T);
    char * ptr = (char*)&t;
    std::string result;
    for(int i = 0;i<sz;i++){
        for(int j =  7 ;j>=0;j--){
            result.push_back((ptr[i] >> j ) & 1 ? '1':'0');
        }
        result.push_back(' ');
    }
    return result;
}

inline void conver(const std::string & s,int& idx,int& bias ,char * buf)
{
    for(int i = 0;i<s.size();i++)
    {
        if(bias == - 1){
            ++idx;
            bias = 7;
        }
        if(s[i] == '1')
        {
            buf[idx] = ((1 << bias) | buf[idx]);
        }
        bias--;
    }
}

void HuffMan::construct(long long * dp) {
    std::unordered_map<char,long long> vals;
    for(int i = 0;i<255;i++) {
        if(dp[i]) {
            vals.insert({static_cast<char>(i),dp[i]});
        }
    }
    std::vector<Node::ptr> nodes;
    for(auto & val : vals){
        Node::ptr newNode(new Node());
        newNode->val = val.second;
        newNode->c = val.first;
        newNode->is_value = true;
        newNode->left = newNode->right = nullptr;
        nodes.push_back(newNode);
    }
    makeTree(nodes);
}

void HuffMan::dfs(const Node::ptr& node,const std::string&  prefix){
    if(node == nullptr) return;
    if(node->is_value){
        encode[node->c] = prefix;
        decode[prefix] = node->c;
    }
    dfs(node->left,prefix+"0");
    dfs(node->right,prefix+"1");
}

void HuffMan::makeTree(const std::vector<Node::ptr> &nodes) {
    using namespace std;
    priority_queue<pair<int,Node::ptr>,vector<pair<int,Node::ptr>>,greater<>> q;
    for(const auto& node : nodes){
        q.push({node->val,node});
    }
    if(q.size() == 1){
        encode[nodes[0]->c] ="0";
        return;
    }

    while(q.size() > 1){
        auto t1 = q.top();q.pop();
        auto t2 = q.top();q.pop();

        Node::ptr newNode(new Node());
        newNode->val = t1.second->val + t2.second->val;
        newNode->left = t1.second;
        newNode->right = t2.second;

        q.push({newNode->val,newNode});
    }
    root = q.top().second;
    dfs(root,"");
}

void Scanner::operator()(const std::string& file_name) {
    std::unordered_map<char,long long> result;
    char read_buf[SCAN_BUF_SIZE];

    int result_of_read = 0;
    long long check_size = 0;

    auto in = open_read_file(file_name);
    // process
    do{
        result_of_read = in.readsome(read_buf,SCAN_BUF_SIZE);
        check_size += result_of_read;
        for(int i = 0;i<result_of_read;i++)
        {
            dp[(unsigned char)read_buf[i]]++;
        }
    }while(result_of_read);

    for(int i = 0;i<=255;i++)
    {
        if(dp[i]){
           // SU_LOG_DEBUG(logger) <<"dp : "<<(char)i <<" "<<dp[i];
            result.insert({(char)i,dp[i]});
        }
    }
    SU_LOG_DEBUG(logger) << " Scanner result size is "<<result.size() ;
}

void Encoder::zip_file(const std::string &zip_file_name) {
    file_info.format_name(zip_file_name);
    scanner(zip_file_name);
    m_tree.construct(scanner.getData());
    file_info.format_map(m_tree.encode);
}


void FileInfo::format_map(const std::unordered_map<char, std::string> &table) {
    SU_LOG_DEBUG(logger) <<" start format_map "<<table.size();
    map = new MapElement[table.size()];

    map_size = table.size();
    memset(map,0,map_size * 4);
    int idx = 0;
    for(auto & it : table) {
        map[idx].length = it.second.size();
        map[idx].key = it.first;
        int convert_idx = 0,convert_bias = 7;
        conver(it.second,convert_idx,convert_bias,map[idx].buf);
        SU_LOG_DEBUG(logger) <<"key : "<<it.first <<" value : "<<it.second<<" buf : "<<get_two_string(map[idx].buf[0]) <<get_two_string(map[idx].buf[1]) ;
        ++idx;
    }
}
