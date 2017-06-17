# shm

nodejs 共享内存本机插件

不再需要安装boost头文件,已经将必要头文件提取（只测试了windows与centos，如果缺少头文件，请安装boost头文件）

安装 npm install chacent

const shm  = require('chacent');

console.log(shm);//可用的常量值定义

let shared = shm('test',shm.open_create,shm.write);

shared.size()

shared.truncate(1024);

shared.remove();

//buffer有大小限制，不要对buffer引用

let buffer = shared.map(shm.write,0,20);

console.log(buffer);
