var shm = require('bindings')('addon');

var obj = shm("test",shm.open_create,shm.write);
obj.truncate(10240);
var buffer = obj.map(shm.write,10);
console.log(buffer,buffer.length);
buffer.write("test");
var buffer2 = obj.map(shm.write);
console.log(buffer2,buffer2.length);
obj.remove();