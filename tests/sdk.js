var addon = require('bindings')('elios_protocol');

const sock = addon("/tmp/test", true);
console.log(sock);
sock.receive(function (data, commande_type) {
  console.log(data);
  console.log(commande_type);
});

const it = setInterval((data) => {
  console.log('interval');
}, 1000)

setInterval(() => {
  sock.send('sdk');
}, 1000)