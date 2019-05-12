#include "Communication.hpp"

#include <thread>

int main()
{
  Communication *connection = new Communication("/tmp/test");

  std::thread receiveThread([&] {
    std::function<void(const protocol_t &, const std::string &)> callBackFunction = [](const protocol_t &header, const std::string &message) {
      std::cout << message << std::endl;
    };
    connection->receive(callBackFunction);
  });

  sleep(1);

  std::thread sendThread([&] {
    connection->send("Hello !", 0);
    
    sleep(1);
    connection->quit();
  });

  sendThread.join();
  receiveThread.join();
  std::cout << "end";
}