#include "Communication.hpp"

Communication::Communication(const std::string &socket_path) {
  _socket_path = socket_path;
  _server_socket_fd = -1;
  _client_socket_fd = -1;

  memset(&_socket_addr, 0, sizeof(_socket_addr));
  _socket_addr.sun_family = AF_UNIX;
  if (*_socket_path.c_str() == '\0') {
    *_socket_addr.sun_path = '\0';
    strncpy(_socket_addr.sun_path + 1, _socket_path.c_str() + 1,
            sizeof(_socket_addr.sun_path) - 2);
  } else {
    strncpy(_socket_addr.sun_path, _socket_path.c_str(),
            sizeof(_socket_addr.sun_path) - 1);
  }
}

Communication::~Communication() {
  if (_server_socket_fd != -1)
    unlink(_socket_path.c_str());
}

void Communication::init_server_socket() {
  std::cout << "Prepare unix socket" << std::endl;

  if ((_server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  if (bind(_server_socket_fd, (struct sockaddr *)&_socket_addr,
           sizeof(_socket_addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(_server_socket_fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }
  std::cout << "Prepare unix socket terminated" << std::endl;
}

void Communication::init_client_socket() {
  if ((_client_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  if (connect(_client_socket_fd, (struct sockaddr *)&_socket_addr,
              sizeof(_socket_addr)) == -1) {
    perror("connect error");
    exit(-1);
  }
}

void Communication::send(const std::string &message) {
  if (_client_socket_fd == -1) {
    init_client_socket();
  }

  int rc = 0;
  int sended = 0;
  protocol_t header;
  header.payload_size = message.size();
  header.command_type = 0;
  const char *toto = message.c_str();

  if (write(_client_socket_fd, &header, sizeof(protocol_t)) <= 0) {
    perror("write error 1");
    exit(-1);
  }
  while (sended < header.payload_size) {
    rc = write(_client_socket_fd, toto + sended, header.payload_size - sended);
    if (rc == -1) {
      perror("write error 2");
      exit(-1);
    }
    sended += rc;
  }
}

void Communication::receive(
    const std::function<void(const protocol_t &, const std::string &)>
        &callback) {
  int cl, rc;
  size_t readed;
  char *buffer;
  protocol_t header;
  if (_server_socket_fd == -1) {
    init_server_socket();
  }

  while (1) {
    if ((cl = accept(_server_socket_fd, NULL, NULL)) == -1) {
      perror("accept error");
      continue;
    }

    readed = 0;
    rc = 0;

    if (read(cl, &header, sizeof(protocol_t)) == -1) {
      perror("read");
      exit(-1);
    }

    buffer = (char *)malloc(sizeof(char) * header.payload_size + 1);

    while (readed < header.payload_size) {
      rc = read(cl, buffer + readed, header.payload_size - readed);
      if (rc == -1) {
        perror("read");
        exit(-1);
      }
      readed += rc;
    }
    buffer[header.payload_size] = 0;

    callback(header, buffer);
    free(buffer);

    close(cl);
  }
}
