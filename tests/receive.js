var addon = require('bindings')('elios_protocol');

const sock = addon("/tmp/test");
const sock2 = addon("/tmp/test2");
console.log(sock);
console.log(sock2);
sock.receive(function (data, commande_type) {
  console.log(data);
  console.log(commande_type);
});


setInterval(() => {
  console.log('cccc');
}, 1000)
