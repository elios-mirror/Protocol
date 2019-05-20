var addon = require('bindings')('elios_protocol');

const sock = addon("/tmp/test", true);
console.log(sock);
sock.receive(function (data, commande_type, reply) {
  console.log(data);
  console.log(commande_type);
});

const it = setInterval((data) => {
  // console.log('interval');
}, 1000)

setInterval(() => {
  // console.log("cc");
  sock.send('Hello Form SDK', 42, (data) => {
    console.log("Message replyed:", data);
  });
}, 2000)