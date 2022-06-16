//
// Created by root on 22-6-14.
//

#include "FileInfo.h"
#include <Log/log.h>

#include <cstring>


void FileInfo::format_map(const std::unordered_map<char, std::string> &table) {
        auto logger = SU_LOG_ROOT();
    SU_LOG_DEBUG(logger) <<" start format_map "<<table.size();
    map = new MapElement[table.size()];
    map_size = table.size();
    memset(map,0,map_size * 4);
    int idx = 0;
    for(auto & it : table) {
        map[idx].length = it.second.size();
        map[idx].key = it.first;
        int convert_idx = 0,convert_bias = 7;
            convert(it.second, convert_idx, convert_bias, map[idx].buf);
        SU_LOG_DEBUG(logger) <<"key : "<<it.first <<" value : "<<it.second<<" buf : "<<get_two_string(map[idx].buf[0]) <<get_two_string(map[idx].buf[1]) ;
        ++idx;
    }
}

/*
 * FileInfo:
 * short name_sz
 * char filename[]
 * unsigned long long file_size
 * short map_size
 * MapElement * map
 */

void FileInfo::output(std::ofstream &out) {
        auto logger = SU_LOG_ROOT();
        SU_LOG_DEBUG(logger) <<"FileInfo out start";
        out.write((char*)&name_sz,sizeof(name_sz));
        out.write(file_name,name_sz);

        out.write((char*)&file_size,sizeof(file_size));

        out.write((char*)&map_size,sizeof(map_size));
        out.write((char*)map,map_size * 4);
        SU_LOG_DEBUG(logger) <<"FileInfo out end";

        auto map_hash_value = check_map_hash();
        std::cout << sizeof(map_hash_value)<<std::endl;
        out.write((char*)&map_hash_value,sizeof(map_hash_value));
}

void FileInfo::input(std::ifstream &in) {
        auto logger = SU_LOG_ROOT();
        in.read((char*)&name_sz,sizeof(name_sz));
        file_name = new char[name_sz + 1];
        file_name[name_sz] = 0;
        in.read(file_name,name_sz);
       SU_LOG_DEBUG(logger) <<" FileInfo : "<<name_sz<<" "<<file_name;

        in.read((char*)&file_size,sizeof(file_size));
        SU_LOG_DEBUG(logger) <<"FileSzie : "<<file_size;

        in.read((char*)&map_size,sizeof(map_size));
        SU_LOG_DEBUG(logger) <<" map_size " << map_size;

        map = (MapElement*) malloc(map_size * 4);
        in.read((char*)map,map_size * 4);
        for(int i = 0;i<map_size;i++)
        {
                SU_LOG_DEBUG(logger) << map[i].key <<" "<<(int)map[i].length <<"  buf : "<<get_two_string(map[i].buf[0]) <<get_two_string(map[i].buf[1]);
        }

        unsigned long long read_value  = -1;
        in.read((char*)&read_value,sizeof read_value);
        auto compute_hash_value = check_map_hash();
        if(read_value != compute_hash_value)
        {
                SU_LOG_ERROR(logger) <<" read_value != compute_hash_value" << read_value << " "<<compute_hash_value ;
               // exit(6);
        }
}

void FileInfo::format_name(const std::string &filename) {
        auto logger = SU_LOG_ROOT();
        name_sz = filename.size();
        file_name = new char[filename.size()+1];
        file_name[name_sz] = 0;
        for(int i = 0;i<filename.size() ;++i) file_name[i] = filename[i];
        file_size = std::filesystem::file_size(file_name);
        SU_LOG_DEBUG(logger) << "FileInof Debug : "<<name_sz<<" :"<<filename <<" : "<<file_size;
}

unsigned long long FileInfo::check_map_hash() {
       unsigned long long result = 0;
       std::string for_test;
       char * ptr = (char*)map;
       for(int i = 0;i<map_size * 4 ;i++) for_test.push_back(ptr[i]);
       result = std::hash<std::string>{}(for_test);
        auto logger = SU_LOG_ROOT();
        SU_LOG_DEBUG(logger) <<"check value " <<result<<std::endl;
       return result;
}

std::unordered_map<std::string, char> FileInfo::getMap() const {
        auto logger = SU_LOG_ROOT();
        std::unordered_map<std::string,char> result;
        for(int i = 0;i<map_size;++i)
        {
                auto key = get_two_string(map[i].buf[0]) + get_two_string(map[i].buf[1]);
                SU_LOG_DEBUG(logger) <<(int)map[i].length;
                std::string insert_key(key.begin(),key.begin() + map[i].length);
                if(result.find(insert_key) != result.end())
                {
                        SU_LOG_DEBUG(logger) << "error"<<insert_key ;
                }
                result.insert({insert_key,map[i].key});
        }
        for(auto it = result.begin() ; it != result.end(); ++it){
                SU_LOG_DEBUG(logger) <<"insert "<<it->second <<" "<<it->first ;
        }
        // todo:
        return result;
}

std::ifstream open_read_file(const std::string & name){
        auto logger = SU_LOG_ROOT();
        std::ifstream result(name,std::ios::binary);
        result >> std::noskipws;
        SU_LOG_DEBUG(logger) <<"try to open file "<<name<<" "<<result.is_open();
        return std::move(result);
}

