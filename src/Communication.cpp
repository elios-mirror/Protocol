#include "Communication.hpp"

Communication::Communication(const std::string &socket_path, bool sdk)
    : _socket_path{socket_path}, _client_socket_path{socket_path},
      _server_socket_path{socket_path}
{
  if (sdk)
  {
    _client_socket_path += "_sdk";
    _server_socket_path += "_mirror";
  }
  else
  {
    _client_socket_path += "_mirror";
    _server_socket_path += "_sdk";
  }

  mkfifo(_client_socket_path.c_str(), 0666);
  mkfifo(_server_socket_path.c_str(), 0666);
  _client_socket_fd = open(_client_socket_path.c_str(), O_RDWR);
  _server_socket_fd = open(_server_socket_path.c_str(), O_RDWR);
}


Communication::~Communication()
{
  quit();
}

void Communication::quit()
{
  _close = true;
}

void Communication::send(const std::string &message, int command_type)
{
  int rc = 0;
  std::size_t sended = 0;

  protocol_t message_header;
  message_header.payload_size = message.size();
  message_header.command_type = command_type;
  const char *buffer = message.c_str();

  if (write(_client_socket_fd, &message_header, sizeof(protocol_t)) <= 0)
  {
    perror("write message_header");
    exit(-1);
  }
  while (sended < message_header.payload_size)
  {
    rc = write(_client_socket_fd, buffer + sended, message_header.payload_size - sended);
    if (rc <= 0)
    {
      perror("write message");
      exit(-1);
    }
    sended += rc;
  }
  if (sended != message_header.payload_size)
  {
    perror("Message not send correcly !");
  }
}

void Communication::receive(
    const std::function<void(const protocol_t &, const std::string &)>
        &callback)
{
  int rc;
  size_t readed;
  char *buffer;
  protocol_t header;

  while (!_close)
  {
    readed = 0;
    rc = 0;

    if (read(_server_socket_fd, &header, sizeof(protocol_t)) == -1)
    {
      perror("read");
      exit(-1);
    }

    buffer = (char *)malloc(sizeof(char) * header.payload_size + 1);
    memset(buffer, 0, header.payload_size + 1);

    while (readed < header.payload_size)
    {
      rc = read(_server_socket_fd, buffer + readed, header.payload_size - readed);

      if (rc <= 0)
      {
        perror("read");
        exit(-1);
      }
      readed += rc;
    }

    callback(header, buffer);
    free(buffer);
  }
}
