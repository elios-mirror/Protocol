#include <napi.h>
#include "Communication.hpp"

Napi::Function cb;
Napi::Env env(NULL);

void ReceiceCallBack(const Napi::CallbackInfo &info)
{
  env = info.Env();
  Communication *protocolInstance = static_cast<Communication *>(info.Data());
  cb = info[0].As<Napi::Function>();
  std::function<void(const protocol_t &, const std::string &)> callBackFunction = [](const protocol_t &header, const std::string &message) {
    cb.Call(env.Global(), {Napi::String::New(env, message.c_str()), Napi::Value::From(env, header.command_type)});
  };
  protocolInstance->receive(callBackFunction);
}

void SendFunction(const Napi::CallbackInfo &info)
{
  Communication *protocolInstance = static_cast<Communication *>(info.Data());
  Napi::String message = info[0].As<Napi::String>();
  protocolInstance->send(message);
}

Napi::Object CreateConnection(const Napi::CallbackInfo &info)
{
  
  Communication *protocolInstance = new Communication(info[0].ToString());
  Napi::Env env = info.Env();
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "socket_path"), info[0].ToString());
  obj.Set(Napi::String::New(env, "receive"), Napi::Function::New(env, ReceiceCallBack, Napi::String::New(env, "receive"), static_cast<void*>(protocolInstance)));
  obj.Set(Napi::String::New(env, "send"), Napi::Function::New(env, SendFunction, Napi::String::New(env, "send"), static_cast<void *>(protocolInstance)));

  return obj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  return Napi::Function::New(env, CreateConnection, "createConnection");
}

NODE_API_MODULE(addon, Init)
