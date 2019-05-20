#pragma once

#include "wrapper.hpp"

typedef struct {
  napi_async_work work;
  napi_threadsafe_function tsfn;
  Communication *protocolInstance;
} ReceiveData;

napi_value ReceiveFunction(napi_env, napi_callback_info);
