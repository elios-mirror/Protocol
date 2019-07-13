var elios_protocol = require(`bindings`)(`elios_protocol`);

const SOCKET_PATH = '/tmp/test';
const MESSAGE_TEXT = '123456789';
const MESSAGE_COMMAND = 42;

let mirror_connection = elios_protocol(`${SOCKET_PATH}`);
let sdk_connection = elios_protocol(`${SOCKET_PATH}`, true);

sdk_connection.receive(() => {console.log("tesr") });

test(`initialize mirror_connection elios_protocol`, () => {
  expect(sdk_connection).not.toBeNull();
});

test(`initialize sdk_connection elios_protocol`, () => {
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
    console.log("init");
    mirror_connection.receive((message, command_type, reply) => {
      console.log(message)
      console.log("reply == " + reply);
      reply(message + "-reply");
      // resolve({message, command_type});

    });
    // mirror_connection.send("cc", 0, () => {

    // });
    // sdk_connection.close();
    // mirror_connection.close();
    // resolve({message: MESSAGE_TEXT + "-reply"})
    setTimeout(() => {
      console.log("timeout")
      sdk_connection.send(MESSAGE_TEXT, MESSAGE_COMMAND, (message) => {
        console.log("replyed");
        console.log("=========");
        sdk_connection.close();
        mirror_connection.close();
        resolve({ message });
      });
    }, 1000);

  }).then((data) => {
    expect(data.message).toBe(MESSAGE_TEXT + "-reply");
  });
});