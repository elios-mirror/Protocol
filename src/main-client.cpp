#include "Communication.hpp"

int main() {
  Communication *test = new Communication("/tmp/elios_mirror", "test", true);

  std::thread tread2([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    test->send("Stop", 42);
    test->quit();
  });

  std::thread tread([&] {
    while (test->canSend()) {
      test->send("Hello", 32);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  });

  tread.detach();
  tread2.detach();

  std::function<void(const protocol_t &, const std::string &,
                     std::function<void(std::string &, int)>)>
      callBackFunction =
          [=](const protocol_t &header, const std::string &message,
              std::function<void(std::string &, int)> replyFunction) {
            std::cout << "Message: " << message << std::endl;
          };

  test->receive(callBackFunction);

  return 0;
}