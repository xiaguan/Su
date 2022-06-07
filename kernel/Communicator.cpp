//
// Created by root on 22-6-3.
//

#include "Communicator.h"
#include <memory.h>
#include <cstdlib>
#include <cerrno>

int CommService::init(const struct sockaddr *bind_addr, socklen_t addrlen,
        int listen_timeout, int response_timeout)
{
    int ret;

    this->bind_addr =  (struct sockaddr *)malloc(addrlen);
    if(this->bind_addr)
    {
      // init the lock
        ret = pthread_mutex_init(&this->mutex,NULL);
        if(ret == 0)
        {
          memcpy(this->bind_addr, bind_addr,addrlen);
          this->listen_timeout = listen_timeout;
          this->response_timeout = response_timeout;
          return 0;
        }
        errno = ret;
        free(this->bind_addr);
  }
  return -1;
}
