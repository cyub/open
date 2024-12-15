#include <stdio.h>
#include <stdlib.h>

static void usage(const char *usage, const char *progname) {
  fprintf(stderr, usage, progname);
  exit(1);
}

static void errExit(const char *msg) {
  perror(msg);
  exit(1);
}

static void fatal(const char *msg) {
  printf("fatal: %s", msg);
  exit(1);
}