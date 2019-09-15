#pragma once

#include <cstring>
#include <iostream>
#include <string>

#include <chrono>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

#include <future>
#include <unordered_map>
#include <vector>

#include <stdexcept>

typedef struct protocol_s {
  std::size_t   payload_size;
  std::size_t   command_type;
  char          sender_id[36];
  int           request_id;
  int           reply_id;
} protocol_t;

typedef struct queue_element_s {
  protocol_t    header;
  std::string   message;
} queue_element_t;

typedef struct connection_s {
  std::thread   thread;
  int fd;
} connection_t;

typedef struct {
  protocol_t    header;
  std::string   message;
  std::function<void(std::string &, int)> replyFunction;
} Payload;

class Communication {
public:
  Communication(const std::string &, const std::string &, bool sdk = false);
  ~Communication();

  void receiveThread(int fd);
  void initServer();

  void initClient();

  std::future<std::string> send(const std::string &, int, int reply_id = -1);
  void
  receive(const std::function<void(const protocol_t &, const std::string &,
                                   std::function<void(std::string &, int)>)> &);
  void quit();
  bool canSend();

private:
  void send_worker();
  void close_connection(const char[36]);
  int _client_socket_fd = -1;
  int _server_socket_fd = -1;
  bool _quit = false;

  std::string _socket_path;
  std::string _sender_id;

  std::queue<queue_element_t> _send_queue;
  std::condition_variable _send_conditional_variable;

  std::unordered_map<int, std::promise<std::string>> _reply_queue;

  std::thread _send_worker_thread;
  std::function<void(const protocol_t &header, const std::string &message,
                     std::function<void(std::string &, int)>)>
      _callback;
};
