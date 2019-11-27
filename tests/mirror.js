var { createConnection } = require('bindings')('elios_protocol');

const connection = createConnection("/tmp/elios_mirror", 'test');

connection.receive(function (data, sender_id, commande_type, reply) {
  console.log(data);
  console.log('sender', sender_id);
  console.log(commande_type);
  switch(commande_type) {
    case 4:
      console.log('reply');
      reply(`[
    {
        "type": "input",
        "name": "firstname",
        "value": "Foo"
    },
    {
        "type": "checkbox",
        "name": "show_media",
        "value": true
    },
    {
        "type": "dropdown",
        "name": "sayhello",
        "value": 2
    }
]`, 5);
      break;
  }
});

const it = setInterval((data) => {
  // console.log('interval');
}, 1000)

// setInterval(() => {
//   connection.send('mirror');
// }, 1000)