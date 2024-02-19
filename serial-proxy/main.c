#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define SERIAL_DEVICE "/dev/ttyS0"
#define TCP_PORT 12345

int main() {
  // 打开串口设备
  int serial_fd = open(SERIAL_DEVICE, O_RDWR | O_NOCTTY);
  if (serial_fd == -1) {
    perror("Error opening serial device");
    return -1;
  }

  // 配置串口设备
  struct termios tty;
  memset(&tty, 0, sizeof(tty));
  if (tcgetattr(serial_fd, &tty) != 0) {
    perror("Error getting serial attributes");
    close(serial_fd);
    return -1;
  }

  // 设置波特率为9600
  cfsetospeed(&tty, B9600);
  cfsetispeed(&tty, B9600);

  // 允许读写，8位数据位，无奇偶校验
  tty.c_cflag |= (CLOCAL | CREAD);
  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  // 禁用软件流控制
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);

  // 设置为原始模式
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // 禁用硬件流控制
  tty.c_cflag &= ~CRTSCTS;

  // 将配置写入串口设备
  if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
    perror("Error setting serial attributes");
    close(serial_fd);
    return -1;
  }

  // 创建 TCP 服务器端口
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("Error creating server socket");
    close(serial_fd);
    return -1;
  }

  // 绑定服务器地址
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(TCP_PORT);

  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) !=
      0) {
    perror("Error binding server socket");
    close(serial_fd);
    close(server_fd);
    return -1;
  }

  // 监听连接
  if (listen(server_fd, 5) != 0) {
    perror("Error listening on server socket");
    close(serial_fd);
    close(server_fd);
    return -1;
  }

  printf("Serial device %s simulated on TCP port %d\n", SERIAL_DEVICE,
         TCP_PORT);

  while (true) {
    // 接受客户端连接
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
      perror("Error accepting client connection");
      continue;
    }

    // 串口设备模拟
    char buff[1024];
    ssize_t bytesRead;

    while ((bytesRead = read(client_fd, buff, sizeof(buff))) > 0) {
      write(serial_fd, buff, bytesRead);
    }

    close(client_fd);
  }

  // 关闭套接字和串口设备
  close(server_fd);
  close(serial_fd);

  return 0;
}