var addon = require('bindings')('elios_protocol');

const sock = addon("/tmp/test");
console.log(sock);
sock.receive(function (data, commande_type, reply) {
  console.log(data);
  console.log(commande_type);
  reply(data + ' Mirror');
});

const it = setInterval((data) => {
  // console.log('interval');
}, 1000)

// setInterval(() => {
//   sock.send('mirror');
// }, 1000)