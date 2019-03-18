#include <napi.h>
#include "Communication.hpp"

Napi::Function cb;
Napi::Env env(NULL);

void RunCallback(const Napi::CallbackInfo &info)
{
  env = info.Env();
  cb = info[0].As<Napi::Function>();
  Communication test("/tmp/test");
  std::function<void(const protocol_t &, const std::string &)> function = [](const protocol_t &header, const std::string &message) {
    cb.Call(env.Global(), {Napi::String::New(env, message.c_str())});
  };
  test.receive(function);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  return Napi::Function::New(env, RunCallback);
}

NODE_API_MODULE(addon, Init)