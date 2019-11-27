var {createConnection} = require('bindings')('elios_protocol');

const connection = createConnection("/tmp/elios_mirror", 'calendar2', true);
console.log(connection);
connection.receive(function (data, sender_id, commande_type, reply) {
  console.log(data);
  console.log(sender_id);
  console.log(commande_type);
});

const it = setInterval((data) => {
  // console.log('interval');
}, 1000)

setInterval(() => {
  // console.log("cc");
  connection.send('Hello Form SDK', 4, (data) => {
    console.log("Message replyed:", data);
  });
}, 2000)