#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define NAPI_EXPERIMENTAL
#include <napi.h>
#include <node_api.h>
#include <node_api_types.h>

#include "Communication.hpp"

typedef struct {
  protocol_t header;
  std::string message;
  std::function<void(std::string &, int)> replyFunction;
} Payload;

// TODO: Rewrite binder C++ -> NODEJS in Real CPP when this issue will be merged
// https://github.com/nodejs/node-addon-api/pull/442 (See main-cpp.cpp for
// example)