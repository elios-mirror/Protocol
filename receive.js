var addon = require('bindings')('elios_protocol');

// addon.receive(function(msg, commande_type){
//   console.log(commande_type);
//   console.log(msg); // 'hello world'
// });

// console.log(addon);

const sock = addon("/tmp/test");
const sock2 = addon("/tmp/test2");
console.log(sock);
console.log(sock2);
sock.receive(function (data) {
  console.log(data);
});