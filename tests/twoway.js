var elios_protocol = require(`bindings`)(`elios_protocol`);

const SOCKET_PATH = '/tmp/test';
const MESSAGE_TEXT = '123456789';
const MESSAGE_COMMAND = 42;

let mirror_connection = elios_protocol(`${SOCKET_PATH}`);
let sdk_connection = elios_protocol(`${SOCKET_PATH}`, true);

console.log("init");
sdk_connection.receive(() => {console.log("tesr") });
mirror_connection.receive((message, command_type, reply) => {
  console.log(message)
  console.log("reply == " + reply);
  reply(message + "-reply");
  // resolve({message, command_type});

});
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
}, 5000);
