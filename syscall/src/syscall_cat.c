/*
 * 使用mmap实现cat的功能
 */
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "include/common.h"

int main(int argc, char *argv[]) {
  int fd;
  struct stat sb;
  char *addr;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usage("%s file\n", argv[0]);
  }

  fd = open(argv[1], O_RDONLY);
  if (fd == -1) errExit("open");

  if (fstat(fd, &sb) == -1) errExit("fstat");

  addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (addr == MAP_FAILED) errExit("mmap");

  if (write(1, addr, sb.st_size) != sb.st_size) fatal("partial/failed write");

  exit(EXIT_SUCCESS);
}
