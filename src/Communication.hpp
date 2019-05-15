#pragma once

#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

typedef struct protocol_s
{
  std::size_t payload_size;
  std::size_t command_type;
} protocol_t;

class Communication
{
public:
  Communication(const std::string &, bool sdk = false);
  ~Communication();
  void send(const std::string &, int command_type);
  void receive(const std::function<void(const protocol_t &, const std::string &)> &);
  void quit();

private:
  void init();
  int _client_socket_fd;
  int _server_socket_fd;

  std::string _socket_path;

  std::string _client_socket_path;
  std::string _server_socket_path;
  bool _close;
  bool _sdk = false;
};
