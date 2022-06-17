//
// Created by root on 22-6-14.
//

#include "Decoder.h"
#include <Thread/BlockQueue.h>
#include <Thread/Outer.h>

#include <cstring>

struct __part_of_file
{
    int buf_size;
    int zip_size;
};


struct decode_thread_task
{
    int order;
    int zip_size;
    int buf_size;
    char * buf;
};

inline void __query_and_convert(char * buf,int idx, int bias ,std::string & query,
                                const std::unordered_map<std::string,char>& table)
{

}

class EasyThreadPool
{
public:
    EasyThreadPool(int thread_num,const std::string & out_file_name,
                   std::unordered_map<std::string,char> & table):
    tasks(10), outer(std::ofstream(out_file_name,std::ios::binary)),table(table){
        for(int i = 0;i<thread_num;i++){
            threads.emplace_back([this, &table](){
                char * out_buf = new char[ENCODER_READ_SIZE + 100];
                auto logger = SU_LOG_ROOT();
                while(true){
                    auto t = tasks.get_front();
                    if(t.order == -1){
                        delete [] out_buf;
                        return ;
                    }
                    SU_LOG_DEBUG(logger) <<"a thread get task "<<t.order ;
                    int idx = 0;
                    memset(out_buf,0,t.buf_size);
                    std::string query;
                    auto buf = t.buf;
                    for(int i = 0;i<t.zip_size;i++)
                    {
                        for(int j = 7;j >= 0;j--){
                            query.push_back((buf[i] >> j) & 1 ? '1':'0');
                            auto it = table.find(query);
                            if(query.size() >= 1000){
                                SU_LOG_DEBUG(logger) << query;
                                exit(66);
                            }
                            if(it == table.end()) continue;
                            if(idx == t.buf_size)
                            {
                                i = t.zip_size;
                                break;
                            }
                            out_buf[idx] = it->second;
                            ++idx;
                            query.clear();
                        }
                    }
                    SU_LOG_DEBUG(logger) <<"a thread finish task "<<t.order ;
                    outer.output(out_buf,idx,t.order);
                }
            });
        }
    }

    void push_new_task(decode_thread_task new_task){tasks.push(new_task);}

    void stop(){
        for(auto & t : threads) t.join();
    }
private:
    std::vector<std::thread> threads;
    BlockQueue<decode_thread_task> tasks;
    std::unordered_map<std::string,char> &table;
    Outer outer;
};


void for_test(std::ifstream & in,const std::unordered_map<std::string,char> & table,const FileInfo & fileInfo)
{
        auto logger = SU_LOG_ROOT();
        int readBytes = 0;
        std::ofstream out("test_"+std::string(fileInfo.file_name),std::ios::binary);
        char * buf = new char[ENCODER_WRITE_SIZE];

        int idx = 0;
        char * out_buf = new char[ENCODER_READ_SIZE];
        do{
                __part_of_file file_part_info{.buf_size = 0,.zip_size = 0};
                in.read((char*)&file_part_info,8);

                idx = 0;

                SU_LOG_DEBUG(logger) <<"buf_size "<<file_part_info.buf_size << " zip_size "<<file_part_info.zip_size - 8;

                in.read(buf,file_part_info.zip_size - 8);
                readBytes = in.gcount();
                SU_LOG_DEBUG(logger) <<"readBytes : "<<readBytes;

                // process
                std::string query;
                for(int i = 0;i<file_part_info.zip_size;i++)
                {
                        for(int j = 7;j >= 0;j--){
                                query.push_back((buf[i] >> j) & 1 ? '1':'0');
                                auto it = table.find(query);
                                if(query.size() >= 1000){
                                        SU_LOG_DEBUG(logger) << query;
                                        exit(66);
                                }
                                if(it == table.end()) continue;
                                if(idx == file_part_info.buf_size)
                                {
                                        i = file_part_info.zip_size;
                                        break;
                                }
                                out_buf[idx] = it->second;
                                ++idx;
                                query.clear();
                        }
                }
                out.write(out_buf,idx);
                SU_LOG_DEBUG(logger) << "output "<<idx <<" ";
        }while(readBytes);
        delete [] out_buf;
        SU_LOG_DEBUG(logger) <<" true file_size "<<fileInfo.file_size;
        SU_LOG_DEBUG(logger) <<" finnaly file_size "<< std::filesystem::file_size("test_"+std::string(fileInfo.file_name));
}


void Decoder::decode(const std::string &decode_file_name) {
        auto logger = SU_LOG_ROOT();
        auto in = open_read_file(decode_file_name);
        fileInfo.input(in);

        auto table = fileInfo.getMap();
        SU_LOG_DEBUG(logger)<< "for_test";

        EasyThreadPool threadPool(15,"test_"+std::string(fileInfo.file_name),table);

        unsigned long long readBytes = 0;
        int order = 0;
        do{
            char * buf = new char[ENCODER_WRITE_SIZE + 100];
            __part_of_file file_part_info{.buf_size = 0,.zip_size = 0};
            in.read((char*)&file_part_info,8);

            in.read(buf,file_part_info.zip_size - 8);
            readBytes = in.gcount();
            decode_thread_task new_task;
            new_task.order = order;
            new_task.zip_size = file_part_info.zip_size;
            new_task.buf_size = file_part_info.buf_size;
            new_task.buf = buf;

            threadPool.push_new_task(new_task);
            ++order;
        }while(readBytes);

        for(int i = 0;i<16;i++) threadPool.push_new_task(decode_thread_task{.order =-1,.buf = nullptr});
        threadPool.stop();
}
