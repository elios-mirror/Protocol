#pragma once

#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

typedef struct protocol_s {
  std::size_t payload_size;
  std::size_t command_type;
} protocol_t;

class Communication {
public:
  Communication(const std::string &);
  ~Communication();
  void send(const std::string &);
  void
  receive(const std::function<void(const protocol_t &, const std::string &)> &);

private:
  struct sockaddr_un _socket_addr;

  int _client_socket_fd;
  int _server_socket_fd;
  std::string _socket_path;
  void init_client_socket();
  void init_server_socket();
};
