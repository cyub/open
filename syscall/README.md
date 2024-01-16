# 系统编程

## 文件操作相关

### open

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```
`open`系统调用的系统调用号是`2`，其`flags`参数常见标志值如下：

标志 | 用途 | 分组 | 注意
--- | --- | --- | ---
O_RDONLY | 以只读方式打开 | 文件访问模式标志
O_WRONLY  | 以只写方式打开 | 
O_RDWR | 以读写方式打开 | 
O_CLOEXEC | 设置close-on-exec标志 | 文件创建标志  | 自Linux 2.6.23版本开始
O_CREAT | 若文件不存在则创建之 |
O_TMPFILE | 用于创建匿名临时文件，当关闭文件描述符时，文件就会自动被删除
O_DIRECTORY | 如果pathname不是目录，则失败
O_EXCL | 结合O_CREAT参数使用，专门用于创建文件，会检查文件是否存在，不存在则新建，否则返回错误信息 |  | **检查文件存在与否和创建文件这两步属于同一原子操作**。可用于任务单进程执行处理
O_TRUNC | 截断已有文件，并将其长度置为零 | 
O_DIRECT | 无缓冲的输入/输出 | 
O_NOFOLLOW | 对符号链接不予解引用 | 
O_NOATIME | 调用read()时，不修改文件最近访问时间 |  | 自 Linux2.6.8版本开始
O_LARGEFILE | 在32位系统中使用此标志打开大文件 | 
O_APPEND  | 总在文件尾部追加数据 | 文件状态控制标志
O_ASYNC | 当I/O操作可行时，产生信号（signal）通知进程。用于信号驱动IO，IO操作会立即返回，同时会在IO操作完成时候发出相应的信号 | | 对正常文件的描述符无效，仅对特定类型的文件有效，诸如终端、FIFOS及socket
O_DSYNC | 提供同步的I/O数据完整性。每一次write操作结束前，都会将文件内容写入硬件（比如磁盘中）|  | 自Linux 2.6.33版本开始
O_NONBLOCK | 以非阻塞方式打开 |
O_SYNC | 以同步方式写入文件。 每一次write操作结束前，都会将文件内容和元信息写入硬件（比如磁盘中） | 

`flags`分组含义：
- **文件访问模式标志：** 用于控制文件访问模式，Open打开文件时候必须使用且只能使用只读、只写、读写中的一个
- **文件创建标志：** 文件创建标志控制的是open和openat在打开文件时的行为
- **文件状态标志：** 文件状态标志控制的是打开文件后的后续IO操作

`open`系统调用的第三个参数`mode`，只有在使用 `O_CREAT` 或 `O_TMPFILE` 标志时才有效，它用来指定新文件的访问权限。`creat`系统调用底层用的`open`:

```go
func Creat(path string, mode uint32) (fd int, err error) {
	return Open(path, O_CREAT|O_WRONLY|O_TRUNC, mode)
}
```

`O_CLOEXEC`需要特别介绍一下。当一个Linux进程使用fork创建子进程后，父进程原有的文件描述符也会复制给子进程。而常见的模式是在fork之后使用exec函数族替换当前进程空间。此时，由于替换前的所有变量都不会被继承，所以文件描述符将丢失，而丢失之后就无法关闭相应的文件描述符，造成泄露。

```c
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("./text.txt", O_RDONLY);
    if (fork() == 0) {
        // child process
        char *const argv[] = {"./child", NULL};
        execve("./child", argv, NULL); // fd left opened
    } else {
        // parent process
        sleep(30);
    }

    return 0;
}
```
上面代码中./child会在启动30秒后会自动退出。在启动这个程序之后，我们使用ps -aux | grep child找到child对应的进程ID，然后使用:

```shell
readlink /proc/xxx/fd/yyy
```
查看，其中xxx为进程ID，yyy是fd中的任意一个文件。我们调查fd中的所有文件，一定能发现一个文件描述符对应text.txt。也就是说，在执行execve之后，子进程始终保持着text.txt的描述符，且没有任何方法关闭它。

解决这个问题的方法一般有两种：

1. 在fork之后，execve之前使用close关闭所有文件描述符。但是如果该进程在此之前创建了许多文件描述符，在这里就很容易漏掉，也不易于维护。
2. 在使用open创建文件描述符时，加入O_CLOEXEC标志位：
    ```c
    int fd = open("./text.txt", O_RDONLY | O_CLOEXEC);
    ```
    通过这种方法，在子进程使用execve时，文件描述符会自动关闭。

### creat

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int creat(const char *pathname, mode_t mode);
```

`creat`系统调用等价于如下`open`调用：
```c
fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, mode);
```

`creat`用来创建文件的系统调用。创建文件的系统调用为啥是creat而不是create？现代Linux系统调用名称大部分是继承自Unix系统。根据《Unix传奇》一书介绍：

> 顺便说一下，creat 系统调用之所以这么拼写，只能归咎于肯 · 汤普森的个人品位，没有其他什么好借口。罗布·派克曾经问肯，如果重写 Unix，他会做哪些修改。他的答案是什么？“我会在 creat 后头加上字母 e。”

顺便提一下，肯 · 汤普森除了发明Unix系统外，他还是Utf-8编码以及Go语言的发明者。

### read

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
```
`read`系统调用用来读取fd文件描述符指向的文件，其系统调用号是0。`read`系统调用的count参数指定最多能读取的字节数。buffer参数提供用来存放输入数据的内存缓冲区地址。缓冲区至少应有count个字节。

如果`read`调用成功，将返回实际读取的字节数，如果遇到文件结束（EOF）则返回0，如果出现错误则返回-1。**一次`read`系统调用读取的字节数可能会小于指定的count数**，读取完成之后，`read`会更新文件偏移，读取多少字节数，就向后移动多少字节。

对于以非阻塞形式打开的文件，还可能返回EAGAIN或EWOULDBLOCK，此时需要`read`调用者不断尝试读取。

使用C语言读取文件时候，应该注意需要将读取的字节数对应地址的缓冲置为空字符`\0`，用来表示读取到的字符串的终止:

```c
#define MAX_READ 20
char buffer[MAX_READ+1]; // 由于空字符占用一个字节的内存，
// 所以缓冲区大小至少要比预计读取的最大字符串多出1个字节
ssize_t numRead;

numRead = read(STDIN_FILENO, buffer, MAX_READ);
if (numRead == -1) {
    perror("read error");
    return;
}

buffer[numRead] = '\0'; // 空字符串
printf("Thead input data was: %s\n", buffer);
```

### write

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```

`write`的系统调用号是1。如果`write`调用成功，将返回实际写入文件的字节数，**该返回值可能小于count参数值。这被称为“部分写”(partial write)**。对磁盘文件来说，造成“部分写”的原因可能是由于磁盘已满，或是因为进程资源对文件大小的限制（RLIMIT_FSIZE限制）。

对磁盘文件执行I/O操作时，`write`调用成功并不能保证数据已经写入磁盘。因为为了减少磁盘活动量和加快`write`系统调用，内核会缓存磁盘的I/O操作，这叫**buffer I/O**。为了避免这种情况，可以使用`sync` 或 `fsync` 系统调用，强制进行落盘操作，或者在`open`创建文件时使用O_SYNC或O_DSYNC标志位来解决这一问题。

同`read`类似，`write`在成功写入n个字节后，会将文件偏移更新n个字节。一般来说，`write`开始写入时的文件偏移就是当前的文件偏移（比如通过`read`读取数据时候更改了偏移或者`lseek`显示的更改了偏移），但是，**当文件描述符是通过open系统调用创建，且创建时使用了O_APPEND标志位的话，每次write开始写入前，都会默认将文件偏移移到文件末尾**。

虽然不能保证数据一定写入硬盘，POSIX标准同样规定了一件事：即使不保证写入硬盘，`read`读入的数据一定是`write`成功之后的数据。

```
After a write() to a regular file has successfully returned:

    Any successful read() from each byte position in the file that was modified by that write shall return the data specified by the write() for that position until such byte positions are again modified.
```
