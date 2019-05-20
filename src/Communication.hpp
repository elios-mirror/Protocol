#pragma once

#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

typedef struct protocol_s {
  std::size_t payload_size;
  std::size_t command_type;
  int request_id;
  int reply_id;
} protocol_t;

typedef struct queue_element_s {
  protocol_t header;
  std::string message;
} queue_element_t;

class Communication {
public:
  Communication(const std::string &, bool sdk = false);
  ~Communication();
  std::future<std::string> send(const std::string &, int, int reply_id = -1);
  void
  receive(const std::function<void(const protocol_t &, const std::string &,
                                   std::function<void(std::string &, int)>)> &);
  void quit();

private:
  void send_worker();

  bool _close = false;

  int _client_socket_fd;
  int _server_socket_fd;

  std::string _socket_path;

  std::string _client_socket_path;
  std::string _server_socket_path;

  std::queue<queue_element_t> _send_queue;
  std::condition_variable _send_conditional_variable;

  std::unordered_map<int, std::promise<std::string>> _reply_queue;
};
