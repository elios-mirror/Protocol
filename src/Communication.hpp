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
#include <queue>
#include <thread>
#include <future>

typedef struct protocol_s
{
  std::size_t payload_size;
  std::size_t command_type;
} protocol_t;


typedef struct queue_element_s
{
  protocol_t header;
  std::string message;
} queue_element_t;

class Communication
{
public:
  Communication(const std::string &, bool sdk = false);
  ~Communication();
  // std::future<std::string> send(const std::string &, int);
  void send(const std::string &, int);
  void receive(const std::function<void(const protocol_t &, const std::string &)> &);
  void quit();

private:
  void send_worker();
  void receive_worker();

  int _client_socket_fd;
  int _server_socket_fd;

  std::string _socket_path;

  std::string _client_socket_path;
  std::string _server_socket_path;

  bool _close = false;

  std::queue<queue_element_t>  _send_queue;
  std::condition_variable _send_conditional_variable;
};
