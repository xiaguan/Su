//
// Created by root on 22-6-3.
//

#include "ServerBase.h"
#include <stdio.h>
#include <errno.h>

///todo
#include <iostream>


static inline int __set_fd_nonblock(int fd)
{
        int flags = fcntl(fd,F_GETFL);
        if(flags >= 0)
                flags = fcntl(fd,F_SETFL,flags | O_NONBLOCK);
        return flags;
}

static int bind_and_listen(int sockfd,const struct sockaddr * addr,socklen_t addrlen)
{
        struct sockaddr_storage ss;
        socklen_t  len;

        len = sizeof(struct sockaddr_storage);
        if (getsockname(sockfd, (struct sockaddr *)&ss, &len) < 0)
        		return -1;
        while (len != 0)
        {
                if (((char *)&ss)[--len] != 0)
                        break;
        }

        if (len == 0)
        {
                if (bind(sockfd, addr, addrlen) < 0)
                        return -1;
        }
        return listen(sockfd, SOMAXCONN);
}

constexpr int PORT_STR_MAX = 5;
int ServerBase::start(int family, const char *host, unsigned short port) {
  struct addrinfo hints = {
      .ai_flags       = AI_PASSIVE,
      .ai_family      = family,
      .ai_socktype    = SOCK_STREAM,
  };
  struct addrinfo * addrinfo;
  char port_str[PORT_STR_MAX+1];

  int ret;
  snprintf(port_str,PORT_STR_MAX+1,"%d",port);
  ret = getaddrinfo(host,port_str,&hints,&addrinfo);
  if(ret == 0)
  {
    start(addrinfo->ai_addr,(socklen_t)addrinfo->ai_addrlen);
  }
  else
  {
    if(ret != EAI_SYSTEM)
          errno = EINVAL;
    ret = -1;
  }
  return ret;
}

int ServerBase::start(const struct sockaddr *bind_addr, socklen_t addrlen) {
  init(bind_addr,addrlen);
  create_listen_fd();
  if(bind_and_listen(this->listen_fd, bind_addr,addrlen) < 0)
          return -1;
  return 0;
}

int ServerBase::init(const struct sockaddr *bind_addr, socklen_t addrlen) {
  int timeout = this->params.peer_response_timeout;
  if(this->CommService::init(bind_addr,addrlen,-1, timeout) <0)
          return -1;

  return 0;
}

int ServerBase::create_listen_fd() {

        this->listen_fd = socket(AF_INET,SOCK_STREAM,0);
        std::cout << this->listen_fd <<"s  s s  "<<errno<<std::endl;
        //__set_fd_nonblock(this->listen_fd);
}


void ServerBase::read_and_cout() {
        char buf[10000];
        memset(buf,0,sizeof buf);
        struct sockaddr  client_addr;
        socklen_t sz = sizeof(client_addr);
        std::cout <<"listen_fd "<<listen_fd<<std::endl;
        int connfd = accept(this->listen_fd,&client_addr,&sz);
        std::cout <<"connfd is "<<connfd<<" "<<errno<<std::endl;
        int ret = read(connfd,buf,10000);
        std::cout <<"ret is "<<ret <<" "<<buf;
}
