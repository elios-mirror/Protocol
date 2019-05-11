#include <napi.h>
#include <chrono>
#include <thread>
#include <iostream>

#include "Communication.hpp"

class EchoWorker : public Napi::AsyncWorker
{
public:
  EchoWorker(Napi::Function &callback, std::string &echo)
      : Napi::AsyncWorker(callback), echo{echo}, callback{callback} {}

  ~EchoWorker() {}
  // This code will be executed on the worker thread
  void Execute()
  {
    // Need to simulate cpu heavy task
    // while (1) {
    // std::cout << "hello" << std::endl;
    // Napi::HandleScope scope(Env());
    // Callback().Call({Env().Null(), Napi::String::New(Env(), echo)});
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << echo << std::endl;
    // Creat a new async context instance.
    Napi::AsyncContext context(Env(), "async_context_test", "");

    // Invoke the callback with the async context instance.
    callback.MakeCallback(Napi::Object::New(Env()),
                          std::initializer_list<napi_value>{}, context);

    // callback.Call(Env().Global(), {Napi::String::New(Env(), "cccc"), Napi::Value::From(Env(), 0)});
    // }
  }

  void OnOK()
  {
    Napi::HandleScope scope(Env());
    Callback().Call({Napi::String::New(Env(), "Hello ;)"), Napi::Value::From(Env(), 0)});
  }

private:
  std::string echo;
  Napi::Function &callback;
};

Napi::Value Echo(const Napi::CallbackInfo &info)
{
  std::cout << "hello" << std::endl;

  // You need to check the input data here
  Napi::Function cb = info[1].As<Napi::Function>();
  std::string in = info[0].As<Napi::String>();
  EchoWorker *wk = new EchoWorker(cb, in);

  wk->Queue();
  return info.Env().Undefined();
}

Napi::Object CreateConnection(const Napi::CallbackInfo &info)
{

  Communication *protocolInstance = new Communication(info[0].ToString());
  Napi::Env env = info.Env();
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "socket_path"), info[0].ToString());
  obj.Set(Napi::String::New(env, "test"), Napi::Function::New(env, Echo, Napi::String::New(env, "test"), NULL));
  // obj.Set(Napi::String::New(env, "send"), Napi::Function::New(env, SendFunction, Napi::String::New(env, "send"), static_cast<void *>(protocolInstance)));

  return obj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  return Napi::Function::New(env, CreateConnection, "createConnection");
}

NODE_API_MODULE(addon, Init)