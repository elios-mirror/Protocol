#include "Communication.hpp"

Communication::Communication(const std::string &socket_path,
                             const std::string &sender_id, bool sdk)
    : _sdk{sdk}, _socket_path{socket_path}, _sender_id{sender_id} {
  _send_worker_thread = std::thread(&Communication::send_worker, this);
}

void Communication::initServer() {
  struct sockaddr_un addr;
  int cl;

  if ((_server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*_socket_path.c_str() == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path + 1, _socket_path.c_str() + 1,
            sizeof(addr.sun_path) - 2);
  } else {
    strncpy(addr.sun_path, _socket_path.c_str(), sizeof(addr.sun_path) - 1);
    unlink(_socket_path.c_str());
  }

  if (bind(_server_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(_server_socket_fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }

  while (_quit != true) {
    if ((cl = accept(_server_socket_fd, NULL, NULL)) == -1) {
      continue;
    }

    std::thread thread(&Communication::receiveThread, this, cl);
    thread.detach();
  }
}

void Communication::close_connection(const char sender_id[36]) {
  if (_callback) {
    protocol_t header;
    header.command_type = 3;
    header.payload_size = 0;
    header.reply_id = -1;
    header.request_id = -1;
    std::strncpy(header.sender_id, sender_id, 36);
    _callback(header, "", nullptr);
  }
}

void Communication::receiveThread(int fd) {
  int rc;
  size_t readed;
  char *buffer;
  protocol_t header;
  char sender_id[36];

  std::memset(sender_id, 0, 36);

  while (!_quit) {
    readed = 0;
    rc = 0;

    if ((rc = read(fd, &header, sizeof(protocol_t))) == -1) {
      perror("read2");
      exit(-1);
    }

    if (rc == 0) {
      // std::cout << "Close communication with " << fd << std::endl;
      close_connection(sender_id);
      return;
    }
    rc = 0;

    std::strncpy(sender_id, header.sender_id, 36);

    buffer = (char *)malloc(sizeof(char) * header.payload_size + 1);
    memset(buffer, 0, header.payload_size + 1);

    while (readed < header.payload_size) {
      if ((rc = read(fd, buffer + readed, header.payload_size - readed)) ==
          -1) {
        perror("read");
        exit(-1);
      }

      if (rc == 0) {
        // std::cout << "Close communication with " << fd << std::endl;
        close_connection(sender_id);
        return;
      }

      readed += rc;
    }

    // if (header.reply_id != -1) {
    //   // if (_reply_queue.find(header.reply_id) != _reply_queue.end()) {
    //   //   _reply_queue.at(header.reply_id).set_value(buffer);
    //   //   _reply_queue.erase(header.reply_id);
    //   // }
    // } else {
    std::function<void(std::string &, int)> replyFunction =
        [&](std::string &message, int commande_type) {
          send(message, commande_type, header.request_id, fd);
        };
    _callback(header, buffer, replyFunction);
    // }

    free(buffer);
  }
}

void Communication::quit() {
  _quit = true;
  if (_server_socket_fd != -1) {
    close(_server_socket_fd);
  }
  if (_client_socket_fd != -1) {
    close(_client_socket_fd);
  }
  _send_conditional_variable.notify_all();
  // if (_send_worker_thread.joinable()) {
  //   _send_worker_thread.join();
  // }
}

void Communication::initClient() {
  struct sockaddr_un addr;

  if ((_client_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*_socket_path.c_str() == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path + 1, _socket_path.c_str() + 1,
            sizeof(addr.sun_path) - 2);
  } else {
    strncpy(addr.sun_path, _socket_path.c_str(), sizeof(addr.sun_path) - 1);
  }

  if (connect(_client_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) ==
      -1) {
    perror("connect error");
    exit(-1);
  }
}

void Communication::send_worker() {
  int rc;
  std::size_t sended;
  std::mutex mutex;
  std::unique_lock<std::mutex> lk(mutex);

  while (!_quit) {
    rc = 0;
    sended = 0;
    _send_conditional_variable.wait(lk, [&] { return !_send_queue.empty(); });
    queue_element_t queue_element = _send_queue.front();
    _send_queue.pop();

    std::string message = queue_element.message;
    protocol_t header = queue_element.header;
    int fd = queue_element.fd;

    const char *buffer = message.c_str();

    if (write(fd, &header, sizeof(protocol_t)) <= 0) {
      perror("write header");
      exit(-1);
    }
    while (sended < header.payload_size) {
      rc = write(fd, buffer + sended, header.payload_size - sended);
      if (rc <= 0) {
        perror("write message");
        exit(-1);
      }
      sended += rc;
    }
    if (sended != header.payload_size) {
      perror("Message not send correcly !");
    }
  }
}

std::future<std::string> Communication::send(const std::string &message,
                                             int command_type, int reply_id,
                                             int reply_fd) {
  if (_quit) {
    return std::promise<std::string>().get_future();
  }
  if (_client_socket_fd == -1)
    initClient();
  queue_element_t queue_element;
  int id = rand() % 2000000;

  queue_element.header.command_type = command_type;
  queue_element.header.payload_size = message.size();
  queue_element.header.request_id = id;
  queue_element.fd = reply_fd != -1 ? reply_fd : _client_socket_fd;
  std::memset(queue_element.header.sender_id, 0, 36);
  std::strncpy(queue_element.header.sender_id, _sender_id.c_str(),
               _sender_id.length() >= 35 ? 35 : _sender_id.length());
  queue_element.header.reply_id = reply_id;
  queue_element.message = message;
  _send_queue.emplace(queue_element);
  _send_conditional_variable.notify_all();
  _reply_queue.emplace(id, std::promise<std::string>());
  return _reply_queue.at(id).get_future();
}

void Communication::receive(
    const std::function<
        void(const protocol_t &header, const std::string &message,
             std::function<void(std::string &, int)>)> &callback) {
  _callback = callback;
  if (_sdk == false) {
    initServer();
  } else {
    if (_client_socket_fd == -1)
      initClient();
    std::thread thread(&Communication::receiveThread, this, _client_socket_fd);
    thread.detach();
    while (_quit != true) {
    }
  }
}

bool Communication::canSend() { return !_quit; }
