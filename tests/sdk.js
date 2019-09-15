var {createConnection} = require('bindings')('elios_protocol');

const connection = createConnection("/tmp/elios_mirror", 'calendar', true);
console.log(connection);
// connection.receive(function (data, command_type, reply) {
//   console.log(data);
//   console.log(command_type);
// });

const it = setInterval((data) => {
  // console.log('interval');
}, 1000)

setInterval(() => {
  // console.log("cc");
  connection.send('Hello Form SDK', 42, (data) => {
    console.log("Message replyed:", data);
  });
}, 2000)