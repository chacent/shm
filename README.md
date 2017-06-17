# shm

nodejs 共享内存本机插件

需要安装boost头文件

const shm  = require('chacent');

console.log(shm);//可用的常量值定义

let shared = shm('test',shm.open_create,shm.write);

shared.size()

shared.truncate(1024);

shared.remove();

//buffer有大小限制，不要对buffer引用

let buffer = shared.map(shm.write,0,20);

console.log(buffer);
