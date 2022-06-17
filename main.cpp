#include <iostream>
#include <memory>
#include <cstring>

#include <Kernel/list.h>
#include <Kernel/thrdpool.h>

using namespace std;


struct listNode{
    listNode *next;
};
struct buf{
    listNode * head;
    int name_len;
    int value_len;
    char * buf;
};

int main(){
        buf * line;
        int name_line = 10;
        int value_len = 20;

        int size = sizeof(buf) + name_line + value_len + 4;
        line = (buf*) malloc(size);

        std::cout << line << " "<<&(line->buf) <<std::endl;
        line->buf = (char*)(line+1);
        std::cout << line << " "<< line + 1<<" "<<(void * )(line->buf) <<std::endl;
        char name[]="Su Jin Ya";
        char value[]="Something Just like";
        memcpy(line->buf,name,name_line);
        line->buf[name_line] = ':';
        line->buf[name_line+1] = ' ';
        memcpy(line->buf+name_line+2,value,value_len);
        line->buf[name_line + 2 + value_len] = '\r';
        line->buf[name_line + 2 + value_len + 1]  = '\n';
        line->name_len = name_line;
        line->value_len = value_len;

        std::cout << line->name_len <<std::endl;
}