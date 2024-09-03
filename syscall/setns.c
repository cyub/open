#define _GNU_SOURCE
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(const char *progname);
void die(const char *msg);
void bindNamespace(pid_t pid, const char *type);
void printInitProgressCommand();
void printHostname();

int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
  }

  pid_t pid = atoi(argv[1]);
  printf("===绑定命令空间前===\n");
  printInitProgressCommand();
  printHostname();

  bindNamespace(pid, "pid"); // 绑定PID命名空间
  bindNamespace(pid,
                "uts"); // 绑定UTS命令空间，此后系统的hostname与绑定的空间一样了
  bindNamespace(
      pid,
      "mnt"); // 绑定MNT命令空间，需要放在最后面，否则提前改变挂载目录，导致其他命名空间文件读取不到
  printf("\n===绑定命令空间后===\n");
  printInitProgressCommand();
  printHostname();
}

void usage(const char *progname) {
  fprintf(stderr,
          "Usage: \n"
          "%s pid(需要绑定到的命名空间的进程id)\n",
          progname);
  exit(EXIT_SUCCESS);
}

void die(const char *msg) {
  perror(msg);
  abort();
}

void bindNamespace(pid_t pid, const char *type) {
  int nstype;
  char filename[100];
  int fd;
  if (strcmp(type, "pid") == 0) {
    snprintf(filename, sizeof(filename), "/proc/%d/ns/pid", pid);
    nstype = CLONE_NEWPID;
  } else if (strcmp(type, "mnt") == 0) {
    snprintf(filename, sizeof(filename), "/proc/%d/ns/mnt", pid);
    nstype = CLONE_NEWNS;
  } else if (strcmp(type, "uts") == 0) {
    snprintf(filename, sizeof(filename), "/proc/%d/ns/uts", pid);
    nstype = CLONE_NEWUTS;
  } else
    die("invalid param");

  fd = open(filename, O_RDONLY);
  if (fd < 0)
    die("open()");

  int rv;
  rv = setns(fd, nstype);
  close(fd);
  if (rv != 0) {
    die("setns()");
  }
}

void printInitProgressCommand() {
  char *initCmdlineFile = "/proc/1/cmdline";
  char cmdline[100] = {0};
  int fd = open(initCmdlineFile, O_RDONLY);
  if (fd < 0)
    die("open()");
  if (read(fd, cmdline, sizeof(cmdline)) < 0) {
    die("read()");
  }

  printf("pid=1 的启动命令: %s\n", cmdline);
}

void printHostname() {
  char hostname[512] = {0};
  if (gethostname(hostname, sizeof(hostname)) < 0) {
    die("gethostname()");
  }
  printf("hostname: %s\n", hostname);
}
