const { createConnection } = require(`bindings`)(`elios_protocol`);

const SOCKET_PATH = '/tmp/test';
const MESSAGE_TEXT = '123456789';
const MESSAGE_COMMAND = 42;


let mirror_connection;
let sdk_connection;

test(`initialize mirror_connection elios_protocol`, () => {
  mirror_connection = createConnection(`${SOCKET_PATH}`);
  expect(sdk_connection).not.toBeNull();
});

test(`initialize sdk_connection elios_protocol`, () => {
  sdk_connection = createConnection(`${SOCKET_PATH}`, true);
  expect(sdk_connection).not.toBeNull();
});

test(`mirror_connection socket_path must be ${SOCKET_PATH}`, () => {
  expect(mirror_connection.socket_path).toBe(`${SOCKET_PATH}`);
});

test(`sdk_connection socket_path must be ${SOCKET_PATH}`, () => {
  expect(mirror_connection.socket_path).toBe(`${SOCKET_PATH}`);
});

test(`mirror_connection must receive message from SDK ${MESSAGE_TEXT}`, () => {
  return new Promise((resolve) => {

    mirror_connection.receive((message, command_type) => {
      resolve({ message, command_type });
    });
    setTimeout(() => {
      sdk_connection.send(MESSAGE_TEXT, MESSAGE_COMMAND);
      sdk_connection.close();
      mirror_connection.close();
    }, 1000);
  }).then((data) => {
    expect(data.message).toBe(MESSAGE_TEXT);
    expect(data.command_type).toBe(MESSAGE_COMMAND);
  });
});