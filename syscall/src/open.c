/**
 * open系统调用示例
 * 来源：https://github.com/libsndfile/libsndfile/blob/1.2.2/src/file_io.c#L581_L595
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define SF_FILENAME_LEN 1024

enum { SFM_READ = 0x10, SFM_WRITE = 0x20, SFM_RDWR = 0x30 };

enum { SFE_NO_ERROR, SFE_BAD_OPEN_MODE };
/*
**	Neat solution to the Win32/OS2 binary file flage requirement.
**	If O_BINARY isn't already defined by the inclusion of the system
**	headers, set it to zero.
*/
#ifndef O_BINARY
#define O_BINARY 0
#endif

typedef struct {
  char path[SF_FILENAME_LEN];
  char dir[SF_FILENAME_LEN];
  char name[SF_FILENAME_LEN / 4];

  int mode;
} PSF_FILE;

static int open_fd(PSF_FILE *file);

int main() {
  PSF_FILE *file;

  if ((file = calloc(1, sizeof(PSF_FILE))) == NULL) {
    perror("calloc");
    return EXIT_FAILURE;
  }

  snprintf(file->path, sizeof(file->path), "%s", "/tmp/open_demo.txt");
  file->mode = SFM_RDWR;
  int fd = open_fd(file);
  if (fd <= 0) {
    perror("open");
    return EXIT_FAILURE;
  } else {
    printf("fd: %d\n", fd);
  }

  close(fd);

  return EXIT_SUCCESS;
}

static int open_fd(PSF_FILE *file) {
  int fd, flag, mode;

  switch (file->mode) {
    case SFM_READ:
      flag = O_RDONLY | O_BINARY;
      mode = 0;
    case SFM_WRITE:
      flag = O_WRONLY | O_CREAT | O_TRUNC | O_BINARY;
      mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
      break;

    case SFM_RDWR:
      flag = O_RDWR | O_CREAT | O_BINARY;
      mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
      break;

    default:
      return -SFE_BAD_OPEN_MODE;
      break;
  };

  if (mode == 0)
    fd = open(file->path, flag);
  else
    fd = open(file->path, flag, mode);

  return fd;
}
