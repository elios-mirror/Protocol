var addon = require('bindings')('elios_protocol');

const sock = addon("/tmp/test");
console.log(sock);
sock.receive(function (data, commande_type) {
  console.log(data);
  console.log(commande_type);
  sock.send("Hello");
});

const it = setInterval((data) => {
  console.log('interval');
  console.log(data);
}, 1000)

setTimeout(() => {
  // sock.close();
  clearInterval(it);
}, 5000)
