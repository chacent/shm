# shm

nodejs 共享内存本机插件

不再需要安装boost头文件,已经将必要头文件提取（只测试了windows与centos，如果缺少头文件，请安装boost头文件）

使用时请注意：

1、内存映射不能太大，也就是map的第三个参数。因为windows系统对映射大小有限制，超出则会物理内存不足。按需取用是基本原则
2、map返回的buffer虽是普通buffer，但不要依赖其引用，因为取消映射依赖于buffer对象的释放，引用会占用物理内存
3、以下仅列出样例，函数参数参考boost(不完全相同，但是有参考意义，有时间再完整的整理下)

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
