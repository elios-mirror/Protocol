#include "Communication.hpp"

int main() {
  Communication* test = new Communication("/tmp/elios_mirror", true);

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
  tread.join();
  tread2.join();

  
  return 0;
}