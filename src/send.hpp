#pragma once

#include "wrapper.hpp"

typedef struct {
  napi_async_work work;
  napi_threadsafe_function tsfn;
  Communication *protocolInstance;
  char *message;
  int command_type;
} SendData;

napi_value SendFunction(napi_env env, napi_callback_info info);