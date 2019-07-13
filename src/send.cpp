#include "send.hpp"

// This function is responsible for converting data coming in from the worker
// thread to napi_value items that can be passed into JavaScript, and for
// calling the JavaScript function.
static void CallJsSend(napi_env env, napi_value js_cb, void *context,
                       void *data) {
  // This parameter is not used.
  (void)context;
  const Payload *payload = (Payload *)data;

  if (env != NULL) {
    napi_value undefined, callback_data[2];

    // Convert the integer to a napi_value.
    napi_create_string_utf8(env, payload->message.c_str(),
                            payload->header.payload_size, &callback_data[0]);

    napi_create_int32(env, payload->header.command_type, &callback_data[1]);

    // Retrieve the JavaScript `undefined` value so we can use it as the `this`
    // value of the JavaScript function call.
    napi_get_undefined(env, &undefined);

    // Call the JavaScript function and pass it the prime that the secondary
    // thread found.
    napi_call_function(env, undefined, js_cb, 2, callback_data, NULL);
  }
  delete (payload);
}

// This function runs on a worker thread. It has no access to the JavaScript
// environment except through the thread-safe function.
static void ExecuteWorkSend(napi_env env, void *data) {
  // We bracket the use of the thread-safe function by this thread by a call to
  // napi_acquire_threadsafe_function() here, and by a call to
  // napi_release_threadsafe_function() immediately prior to thread exit.
  static SendData *addon_data = (SendData *)data;
  napi_acquire_threadsafe_function(addon_data->tsfn);

  std::string message{addon_data->message};
  std::future<std::string> promise =
      addon_data->protocolInstance->send({message}, addon_data->command_type);
  promise.wait();

  std::string reponse = promise.get();

  protocol_t header; // TODO Add header infos
  header.command_type = 0;
  header.payload_size = reponse.size();
  header.reply_id = -1;
  header.request_id = -1;
  const Payload *payload = new Payload{header, reponse, nullptr};

  napi_call_threadsafe_function(addon_data->tsfn, (void *)payload,
                                napi_tsfn_blocking);

  // Indicate that this thread will make no further use of the thread-safe
  // function.
  napi_release_threadsafe_function(addon_data->tsfn, napi_tsfn_release);
}

// This function runs on the main thread after `ExecuteWork` exits.
static void WorkCompleteSend(napi_env env, napi_status status, void *data) {
  SendData *addon_data = (SendData *)data;

  // Clean up the thread-safe function and the work item associated with this
  // run.
  napi_release_threadsafe_function(addon_data->tsfn, napi_tsfn_release);
  napi_delete_async_work(env, addon_data->work);

  // Set both values to NULL so JavaScript can order a new run of the thread.
  addon_data->work = NULL;
  addon_data->tsfn = NULL;
}

// Create a thread-safe function and an async queue work item. We pass the
// thread-safe function to the async queue work item so the latter might have a
// chance to call into JavaScript from the worker thread on which the
// ExecuteWork callback runs.
napi_value SendFunction(napi_env env, napi_callback_info info) {
  napi_status status;
  int command_type = 0;
  size_t argc = 3;
  size_t len;
  napi_value args[argc];
  napi_value work_name;
  SendData *addon_data;

  status =
      napi_get_cb_info(env, info, &argc, args, nullptr, (void **)(&addon_data));

  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "Wrong number of arguments");
    return nullptr;
  }

  // Get the length of the string by passing NULL as the buf
  status = napi_get_value_string_utf8(env, args[0], NULL, 0, &len);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Could not get string length");
  }

  char *message =
      (char *)malloc(sizeof(char) * (len + 1)); // +1 for null termination
  if (message == NULL) {
    perror("malloc SendFunction");
    exit(-1);
  }
  status = napi_get_value_string_utf8(env, args[0], message, len + 1, &len);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Could not extract string");
  }

  if (argc >= 2) {
    status = napi_get_value_int32(env, args[1], &command_type);
    if (status != napi_ok) {
      napi_throw_error(env, NULL, "Could not extract int");
    }
  }

  // Create a string to describe this asynchronous operation.
  napi_create_string_utf8(env, "Send function with callback", NAPI_AUTO_LENGTH,
                          &work_name);

  if (argc >= 3) {

    // Convert the callback retrieved from JavaScript into a thread-safe
    // function
    // which we can call from a worker thread.
    status = napi_create_threadsafe_function(env, args[2], NULL, work_name, 0,
                                             1, NULL, NULL, NULL, CallJsSend,
                                             &(addon_data->tsfn));
    if (status != napi_ok) {
      napi_throw_error(env, NULL, "Could not extract callback function");
    }
  }

  addon_data->message = message;
  addon_data->command_type = command_type;

  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Could not extract int");
  }
  // Create an async work item, passing in the addon data, which will give the
  // worker thread access to the above-created thread-safe function.
  napi_create_async_work(env, NULL, work_name, ExecuteWorkSend,
                         WorkCompleteSend, addon_data, &(addon_data->work));

  // Queue the work item for execution.
  napi_queue_async_work(env, addon_data->work);

  return NULL;
}