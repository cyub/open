# lsh

最简单shell实现。原始来源：[Tutorial - Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/)

内置实现了`cd`,`exit`,`help`命令。当前目录是进程的属性，`cd`若下放到子进程执行，它无法更改父进程的目录。同样`exit`，必须要内置到shell里面。