//
// Created by root on 22-6-3.
//

#ifndef SU_SERVERBASE_H
#define SU_SERVERBASE_H

#include "../kernel/Communicator.h"

#include <mutex>
#include <condition_variable>

struct ServerParams
{
  size_t max_connections;
  int peer_response_timeout;
  int receive_timeout;
  int keep_alive_timeout;
  size_t request_size_limit;
};

static constexpr struct ServerParams DEFAULT_PARAMS =
{
        .max_connections =    2000,
        .peer_response_timeout = 10*1000,
        .receive_timeout = -1,
        .keep_alive_timeout = 60 * 1000,
        .request_size_limit = (size_t)-1,
};

class ServerBase : public CommService{
public:
  // CommService did nothing
  explicit ServerBase(const struct ServerParams * params = &DEFAULT_PARAMS) : CommService()
  {
    this->params = *params;
  }

  int start(unsigned  short port)
  {
      return start(AF_INET,nullptr,port);
  }

  int start(int family,const char * host,unsigned  short port);

  int start(const struct sockaddr* bind_addr,socklen_t addrlen);
  void stop();


  void read_and_cout();

public:
  // get method

protected:
  ServerParams params;


private:
  int init(const struct  sockaddr * bind_addr,socklen_t addrlen);

  virtual int create_listen_fd();

private:
  int listen_fd;

  std::mutex mutex;
  std::condition_variable cond;

public:
  virtual ~ServerBase() = default;
};

#endif // SU_SERVERBASE_H
