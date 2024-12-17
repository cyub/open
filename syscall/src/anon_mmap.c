#ifdef USE_MAP_ANON
#define _BSD_SOURCE
#endif

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wait.h>

#include "include/common.h"

int main(int argc, char *argv[]) {
  int *addr;
#ifdef USE_MAP_ANON
  addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0);
  if (addr == MAP_FAILED) {
  }
#else
  int fd;
  fd = open("/dev/zero", O_RDWR);
  if (fd == -1) {
    errExit("open");
  }
  addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    errExit("mmap");
  }
  if (close(fd) == -1) {  // 此时fd不再需要了
    errExit("close");
  }

#endif

  *addr = 1;
  switch (fork()) {
    case -1:
      errExit("fork");
      break;
    case 0:
      printf("child: %d\n", *addr);
      (*addr)++;
      if (munmap(addr, sizeof(int)) == -1) {
        errExit("munmap");
      }
      exit(EXIT_SUCCESS);
    default:
      if (wait(NULL) == -1)  // 等待子进程退出
        errExit("wait");
      printf("parent: %d\n", *addr);
      if (munmap(addr, sizeof(int)) == -1) {
        errExit("munmap");
      }
      exit(EXIT_SUCCESS);
  }
}