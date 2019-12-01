#include "Communication.hpp"

int main() {
  Communication *test = new Communication("/tmp/elios_mirror", "mirror", false);

  std::thread tread([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    std::cout << "Close " << std::endl;
    test->quit();
  });

  tread.detach();

  std::function<void(const protocol_t &, const std::string &,
                     std::function<void(std::string &, int)>)>
      callBackFunction =
          [=](const protocol_t &header, const std::string &message,
              std::function<void(std::string &, int)> replyFunction) {
            std::cout << "Message: " << message << std::endl;
            std::string response(message + " TOTO");
            replyFunction(response, 32);
          };

  test->receive(callBackFunction);

  return 0;
}