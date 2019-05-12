var addon = require('bindings')('elios_protocol');

const sock = addon("/tmp/test");
console.log(sock);

setInterval(() => {
  sock.send('toto');
}, 1000)