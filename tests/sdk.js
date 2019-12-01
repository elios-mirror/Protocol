var {createConnection} = require('bindings')('elios_protocol');

const connection = createConnection("/tmp/elios_mirror", 'calendar2', true);
console.log(connection);
connection.receive(function (data, sender_id, commande_type, reply) {
  console.log(data);
  console.log(sender_id);
  console.log(commande_type);
});

const it = setInterval(() => {
  // console.log("cc");
  connection.send('Hello Form SDK', 4, (data) => {
    console.log("Message replyed:", data);
  });
}, 2000)


setTimeout(() => {
  clearInterval(it);
  console.log('Close');
  connection.close();
}, 5000);