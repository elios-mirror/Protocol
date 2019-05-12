var elios_protocol = require('bindings')('elios_protocol');

const MESSAGE_TEXT = "123456789";
const MESSAGE_COMMAND = 42;

let connection;

test('initialize elios_protocol', () => {
  connection = elios_protocol('/tmp/test');
  expect(connection).not.toBeNull();
});

test('socket_path must be /tmp/test', () => {
  expect(connection.socket_path).toBe("/tmp/test");
});

test(`receive message must be ${MESSAGE_TEXT}`, () => {
  return new Promise((resolve) => {
    connection.receive((message, command_type) => {
      resolve({ message, command_type });
    });
    setTimeout(() => {
      connection.send(MESSAGE_TEXT, MESSAGE_COMMAND);
      connection.close();
    }, 1000);
  }).then((data) => {
    expect(data.message).toBe(MESSAGE_TEXT);
    expect(data.command_type).toBe(MESSAGE_COMMAND);
  });
});