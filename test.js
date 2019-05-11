// Use the "bindings" package to locate the native bindings.
const binding = require('bindings')('elios_protocol');


console.log(binding);

const test = binding('/tmp/test');
// const test = binding();
console.log(test);

test.receive((message, header) => {
  console.log("Header: \n ", header);
  console.log("Message: \n ", message);
});

setInterval(() => {
  test.send("Hello tout le monde !\n", 34);
}, 1000);