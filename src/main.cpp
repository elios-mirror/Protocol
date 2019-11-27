#include <chrono>
#include <napi.h>
#include <thread>

#include "Communication.hpp"

std::thread nativeThread;
Napi::ThreadSafeFunction tsfn;

Napi::Value Reply(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() < 2) {
    Napi::TypeError::New(env, "Expected two arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  } else if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Expected first arg to be string")
        .ThrowAsJavaScriptException();
    return env.Null();
  } else if (!info[1].IsNumber()) {
    Napi::TypeError::New(env, "Expected second arg to be number")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  Payload *payload = static_cast<Payload *>(info.Data());
  std::string replyMessage(info[0].ToString());
  payload->replyFunction(replyMessage, info[1].ToNumber());
  return Napi::Boolean::New(env, true);
}

Napi::Value Receive(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Expected one arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  } else if (!info[0].IsFunction()) {
    Napi::TypeError::New(env, "Expected first arg to be function")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  Communication *protocolInstance = static_cast<Communication *>(info.Data());

  // Create a ThreadSafeFunction
  tsfn = Napi::ThreadSafeFunction::New(
      env,
      info[0].As<Napi::Function>(), // JavaScript function called asynchronously
      "Resource Name",              // Name
      0,                            // Unlimited queue
      1,                            // Only one thread will use this initially
      [](Napi::Env) {               // Finalizer used to clean threads up
        nativeThread.join();
      });

  // Create a native thread
  nativeThread = std::thread([=] {
    auto callback = [](Napi::Env env, Napi::Function jsCallback,
                       const Payload *payload) {

      // Transform native data into JS data, passing it to the provided
      // `jsCallback` -- the TSFN's JavaScript function.
      jsCallback.Call({Napi::String::New(env, payload->message),
                       Napi::String::New(env, payload->header.sender_id),
                       Napi::Number::New(env, payload->header.command_type),
                       Napi::Function::New(env, Reply, Napi::String::New(env, "reply"), (void*)payload)});

      // We're finished with the data.
      delete payload;
    };

    std::function<void(const protocol_t &, const std::string &,
                       std::function<void(std::string &, int)>)>
        callBackFunction = [=](const protocol_t &header,
                               const std::string &message,
                               std::function<void(std::string &, int)>
                                   replyFunction) {
          const Payload *payload = new Payload{header, message, replyFunction};
          napi_status status = tsfn.BlockingCall(payload, callback);
          if (status != napi_ok) {
            perror("Error tsfn");
          }
        };

    protocolInstance->receive(callBackFunction);
    // Release the thread-safe function
    tsfn.Release();
  });

  return Napi::Boolean::New(env, true);
}

Napi::Value Send(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() < 2) {
    Napi::TypeError::New(env, "Expected two arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  } else if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Expected first arg to be string")
        .ThrowAsJavaScriptException();
    return env.Null();
  } else if (!info[1].IsNumber()) {
    Napi::TypeError::New(env, "Expected second arg to be number")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  Communication *protocolInstance = static_cast<Communication *>(info.Data());
  protocolInstance->send(info[0].ToString(), info[1].ToNumber());
  return Napi::Boolean::New(env, true);
}

Napi::Value Close(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Communication *protocolInstance = static_cast<Communication *>(info.Data());
  protocolInstance->quit();
  return Napi::Boolean::New(env, true);
}

Napi::Object CreateConnection(const Napi::CallbackInfo &info) {

  Napi::Env env = info.Env();
  Napi::Object obj = Napi::Object::New(env);

  if (info.Length() <= 1) {
    Napi::TypeError::New(env, "Expected one argument")
        .ThrowAsJavaScriptException();
    return obj;
  } else if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Expected first arg to be string")
        .ThrowAsJavaScriptException();
    return obj;
  } else if (!info[1].IsString()) {
    Napi::TypeError::New(env, "Expected second arg to be string")
        .ThrowAsJavaScriptException();
    return obj;
  } else if (info.Length() > 2 && !info[2].IsBoolean()) {
    Napi::TypeError::New(env, "Expected second arg to be boolean")
        .ThrowAsJavaScriptException();
    return obj;
  }

  Communication *protocolInstance = new Communication(
      info[0].ToString(), info[1].ToString(), info[2].ToBoolean());

  obj.Set(Napi::String::New(env, "socket_path"), info[0].ToString());
  obj.Set(Napi::String::New(env, "sender_id"), info[1].ToString());
  obj.Set(Napi::String::New(env, "is_sdk"),
          info.Length() > 2 ? info[2].ToBoolean() : false);
  obj.Set(Napi::String::New(env, "receive"),
          Napi::Function::New(env, Receive, Napi::String::New(env, "receive"),
                              static_cast<void *>(protocolInstance)));
  obj.Set(Napi::String::New(env, "send"),
          Napi::Function::New(env, Send, Napi::String::New(env, "send"),
                              static_cast<void *>(protocolInstance)));
  obj.Set(Napi::String::New(env, "close"),
          Napi::Function::New(env, Close, Napi::String::New(env, "close"),
                              static_cast<void *>(protocolInstance)));

  return obj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("createConnection", Napi::Function::New(env, CreateConnection));
  return exports;
}

NODE_API_MODULE(clock, Init)