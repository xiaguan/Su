//
// Created by root on 22-6-14.
//

#include "Decoder.h"
#include <assert.h>

struct __part_of_file
{
    int buf_size;
    int zip_size;
};



void for_test(std::ifstream & in,const std::unordered_map<std::string,char> & table,const FileInfo & fileInfo)
{
        auto logger = SU_LOG_ROOT();
        int readBytes = 0;
        std::ofstream out("test_"+std::string(fileInfo.file_name));

        std::string file_name = "test_"+std::string(fileInfo.file_name);
        char * buf = new char[ENCODER_WRITE_SIZE];
        int idx = 0;
        char * out_buf = new char[ENCODER_READ_SIZE];
        do{
                __part_of_file file_part_info{.buf_size = 0,.zip_size = 0};
                in.read((char*)&file_part_info,8);

                int idx = 0;

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
        delete out_buf;
        SU_LOG_DEBUG(logger) <<" true file_size "<<fileInfo.file_size;
        SU_LOG_DEBUG(logger) <<" finnaly file_size "<< std::filesystem::file_size("test_"+std::string(fileInfo.file_name));
}

void test_file_content(std::ifstream & in){
        const int test_num = 1e5;
        char * buf = new char;
        in.read(buf,1e5);
        auto logger = SU_LOG_ROOT();
        for(int i = 0;i<test_num;i++){
                SU_LOG_DEBUG(logger) << get_two_string(buf[i]);
        }
}

void Decoder::decode(const std::string &file_name) {
        auto logger = SU_LOG_ROOT();
        auto in = open_read_file(file_name);
        assert(in.is_open());
        fileInfo.input(in);

        auto table = fileInfo.getMap();
        SU_LOG_DEBUG(logger)<< "for_test";
        for_test(in,table,fileInfo);
}
