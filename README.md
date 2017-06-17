# shm

nodejs 共享内存本机插件

需要安装boost头文件
const shm  = require('chacent');
console.log(shm);//可用的常量值定义
let shared = shm('test',shm.open_create,shm.write);
shared.size()
shared.truncate(1024);
shared.remove();
let buffer = shared.map(shm.write);
console.log(buffer);
