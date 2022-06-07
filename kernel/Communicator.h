//
// Created by root on 22-6-3.
//

#ifndef SU_COMMUNICATOR_H
#define SU_COMMUNICATOR_H


#include <cstring>

#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <fcntl.h>

class CommService{
public:
    CommService() = default;
    // to do : fix the funcs
    int init(const struct sockaddr *bind_addr,socklen_t addrlen,
            int listen_timeout,int response_timeout) ;

    void deinit();

    void get_addr(struct sockaddr **addr,socklen_t * addrlen) const
    {
            *addr = this->bind_addr;
            *addrlen = this->addrlen;
    };


    /*
     * here are private dates about the Comm
     */
private:
    struct sockaddr * bind_addr;
    socklen_t addrlen;
    int listen_timeout;
    int response_timeout;

private:
    int listen_fd;

private:
  pthread_mutex_t mutex;

public:
   virtual ~CommService() {};

};

#endif //SU_COMMUNICATOR_H
