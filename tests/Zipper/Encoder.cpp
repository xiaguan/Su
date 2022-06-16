//
// Created by susun on 2022/6/9.
//

#include "Encoder.h"
#include "FileInfo.h"

#include <Thread/Outer.h>
#include <Thread/BlockQueue.h>


#include <filesystem>
#include <queue>

#include <assert.h>



inline auto logger = SU_LOG_ROOT();
void HuffMan::construct(long long * dp) {
    std::unordered_map<char,long long> vals;
    for(int i = 0;i<=255;i++) {
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
    char* read_buf = new char[SCAN_BUF_SIZE];

    int result_of_read = 0;
    long long check_size = 0;

    auto in = open_read_file(file_name);
    // process
    do{
        in.read(read_buf,SCAN_BUF_SIZE);
        result_of_read = in.gcount();
        check_size += result_of_read;
        for(int i = 0;i<result_of_read;i++)
        {
            dp[(unsigned char)read_buf[i]]++;
        }
        SU_LOG_DEBUG(logger) << " " << result_of_read << std::endl;
    }while(result_of_read);
    for(int i = 0;i<=255;i++)
    {
        if(dp[i]){
            SU_LOG_DEBUG(logger) <<"dp : "<<(char)i <<" "<<dp[i];
            result.insert({(char)i,dp[i]});
        }
    }
    SU_LOG_DEBUG(logger) << " Scanner result size is "<<result.size() ;
    delete read_buf;
}

void file_outer(std::unordered_map<char,std::string> & encode,FileInfo & fileInfo,std::ofstream & out)
{
    std::ifstream in = open_read_file(fileInfo.file_name);
    int readBytes = 0;
    long long file_test = 0;
    char* read_buf = new char[ENCODER_READ_SIZE];
    char* out_buf = new char[ENCODER_WRITE_SIZE];
    int idx = 0,bias = 7;
    do{
            in.read(read_buf,ENCODER_READ_SIZE);
            readBytes = in.gcount();
            file_test += readBytes;
            for(int i = 0;i<readBytes ; i++){
                auto s = encode[read_buf[i]];
                    convert(s, idx, bias, out_buf);
            }
            out.write(out_buf,idx);
            SU_LOG_DEBUG(logger) << readBytes <<" "<<idx<<" "<<bias;
            memset(out_buf,0,idx);
            idx = 0,bias = 7;
    }while(readBytes);
    SU_LOG_DEBUG(logger) <<"debug filesize : "<<file_test<<" "<<fileInfo.file_size;
}

//todo
#include <fcntl.h>

class FileFormatter{
public:
    FileFormatter(const std::string & file_name,const std::unordered_map<char,std::string> & table)
    :table(table), outer(std::ofstream(file_name + ".huf",std::ios::binary | std::ios::app))
    {
            in = open_read_file(file_name);
    }

    void start(int threads_nums)
    {
        std::vector<std::thread> threads;
        BlockQueue<thread_task> task_queue(10);
        for(int i = 0;i<threads_nums;i++)
        {
                threads.emplace_back([&task_queue,this](){
                        int test = 0;
                        char* out_buf = new char[ENCODER_WRITE_SIZE];
                        while(true){
                                memset(out_buf, 0, ENCODER_WRITE_SIZE);
                                SU_LOG_DEBUG(logger) <<"one thread process "<<++test;
                                auto t = task_queue.get_front();
                                if(t.order == -1) {
                                        delete [] out_buf;
                                        return;
                                }
                                if(t.getBytes == 0) {
                                        delete t.read_buf;
                                        continue;
                                }
                                SU_LOG_DEBUG(logger) <<"get task "<<t.order;
                                int idx = 8,bias = 7;
                                for(int i = 0;i<t.getBytes;i++){
                                        auto it = table.find(t.read_buf[i]);
                                        if(it == table.end()){
                                                SU_LOG_DEBUG(logger) <<"error "<<t.read_buf[i];
                                        }
                                        convert(it->second,idx,bias,out_buf);
                                }
                                part_of_file part_file_info{.buf_size = t.getBytes, .zip_size = idx+1};
                                memcpy(out_buf,&part_file_info,8);
//                                SU_LOG_DEBUG(logger) << part_file_info.zip_size <<" "<<part_file_info.buf_size <<" "<<get_two_string(out_buf[0])<<get_two_string(out_buf[1])<<get_two_string(out_buf[2])<<get_two_string(out_buf[3]);
//                                SU_LOG_DEBUG(logger )<< get_two_string(part_file_info.buf_size);
                                this->outer.output(out_buf,part_file_info.zip_size,t.order);
                                SU_LOG_DEBUG(logger) <<"wait out put doen " <<t.order<<" "<<part_file_info.buf_size<<" "<< part_file_info.zip_size << " "<<ENCODER_READ_SIZE;
                                delete t.read_buf;
                        }
                });
        }

        char * in_buf = new char[ENCODER_READ_SIZE];
        int readBytes = 0;
        int order = 0;
        do{
                in.read(in_buf,ENCODER_READ_SIZE);
                readBytes = in.gcount();
                SU_LOG_DEBUG(logger) <<" readBytes : "<<readBytes;
                thread_task new_task{
                        .order = order,
                        .read_buf = in_buf,
                        .getBytes = readBytes};
                task_queue.push(new_task);
                in_buf = new char[ENCODER_READ_SIZE];
                ++order;
        }while(readBytes);
        for(int i = 0;i<threads_nums;i++) task_queue.push(thread_task{.order = -1,.read_buf = nullptr,.getBytes = 0});
        for(auto & t : threads ) t.join();
        SU_LOG_DEBUG(logger) <<"OutputSize : "<< outer.size();
    }
private:
    std::ifstream in;
    std::unordered_map<char,std::string> table;
    Outer outer;
    struct part_of_file
    {
        int buf_size;
        int zip_size;
    };

    struct thread_task
    {
        int order;
        char * read_buf;
        int getBytes;
    };
    std::mutex m_mut;
};



void Encoder::zip_file(const std::string &zip_file_name) {
    SU_LOG_DEBUG(logger) <<"start";
    file_info.format_name(zip_file_name);

    scanner(zip_file_name);
    m_tree.construct(scanner.getData());
    file_info.format_map(m_tree.encode);

    std::ofstream out(std::string(file_info.file_name) + ".huf",std::ios::binary);
    assert(out.is_open());
    file_info.output(out);
    out.close();

    FileFormatter fileFormatter(zip_file_name,m_tree.encode);

    int thread_num = std::thread::hardware_concurrency();
    if(thread_num == 0) thread_num = 1;
    else --thread_num;
    fileFormatter.start(thread_num);
    auto after_size = std::filesystem::file_size(zip_file_name + ".huf");
    std::cout <<" zip done "<< (double)after_size / (double)file_info.file_size <<std::endl;
}




