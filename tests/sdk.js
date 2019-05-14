var addon = require('bindings')('elios_protocol');

const sock = addon("/tmp/test");
console.log(sock);
sock.receive(function (data, commande_type) {
  console.log(data);
  console.log(commande_type);
});

const it = setInterval((data) => {
  console.log('interval');
  console.log(data);
}, 1000)

setInterval(() => {
  sock.send('toto');
}, 1000)