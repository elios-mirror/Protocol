const { createConnection } = require('bindings')('elios_protocol');

const connection = createConnection('/tmp/elios_mirror')

console.log(connection);

connection.receive(function (data, command_type, reply) {
    console.log("JavaScript callback called with arguments", data, command_type, reply);

    if (command_type == 42) {
        connection.close();
    }
});

console.log('App stated');