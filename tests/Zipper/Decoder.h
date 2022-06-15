//
// Created by root on 22-6-14.
//

#ifndef SU_DECODER_H
#define SU_DECODER_H

#include "FileInfo.h"
#include <Log/log.h>
#include <fstream>

class Decoder {
public:
        Decoder(){
                auto logger = SU_LOG_ROOT();
                logger->setFileAppender("decoder_log.txt");
        }
        void decode(const std::string & file_name);
private:
    std::string file_name;
    FileInfo fileInfo;
};


#endif //SU_DECODER_H
