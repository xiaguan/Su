#include <Log/log.h>
auto logger = SU_LOG_ROOT();
int main(){
    logger->setFileAppender("test.txt");
    SU_LOG_DEBUG(logger) <<"hello world!";
}