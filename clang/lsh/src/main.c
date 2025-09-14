#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define LINE_TOKEN_INIT_SIZE 5
#define LINE_SPILT_DELIM " \n\t"

void lsh_loop();
char *lsh_get_line();
char **lsh_get_args(char *);
int lsh_launch(char **);
int lsh_execute(char **);
int lsh_builtin_cd(char **);
int lsh_builtin_exit(char **);
int lsh_builtin_help(char **);
int lsh_builtin_cmd_num();

typedef int (*builtin_cmd_handle)(char **);

typedef struct {
  char *name;
  builtin_cmd_handle handle;
} builtin_cmd;

builtin_cmd builtin_cmds[] = {{"cd", lsh_builtin_cd},
                              {"help", lsh_builtin_help},
                              {"exit", lsh_builtin_exit}};

int main() {
  setbuf(stdout, NULL);
  lsh_loop();
  return EXIT_SUCCESS;
}

void lsh_loop() {
  int status = 0;
  char *line = NULL;
  char **args = NULL;
  do {
    printf("> ");
    line = lsh_get_line();
    args = lsh_get_args(line);
    status = lsh_launch(args);
    free(line);
    free(args);
  } while (status);
}

char *lsh_get_line() {
  char *line = NULL;
  size_t size;
  if (getline(&line, &size, stdin) == -1) {
    if (feof(stdin)) {
      printf("\nbye.\n");
      exit(EXIT_SUCCESS);
    } else {
      printf("some error");
      exit(EXIT_FAILURE);
    }
  }
  return line;
}

char **lsh_get_args(char *line) {
  char **args = malloc(LINE_TOKEN_INIT_SIZE * sizeof(char *));
  if (!args) {
    perror("malloc args error");
    exit(EXIT_FAILURE);
  }

  char *token = strtok(line, LINE_SPILT_DELIM);
  int position = 0;
  size_t size = LINE_TOKEN_INIT_SIZE;
  char **args_backup;
  while (token) {
    args[position] = token;
    position++;
    if (position >= size) {
      size *= 2;
      args_backup = args;
      args = realloc(args, size);
      if (!args) {
        free(args_backup);
        perror("realloc args error");
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, LINE_SPILT_DELIM);
  }

  args[position] = NULL;
  return args;
}

int lsh_launch(char **args) {
  if (args[0] == NULL) {
    return 1;
  }

  for (int i = 0; i < lsh_builtin_cmd_num(); i++) {
    if (strcmp(args[0], builtin_cmds[i].name) == 0) {
      return builtin_cmds[i].handle(args);
    }
  }
  return lsh_execute(args);
}

int lsh_builtin_cmd_num() { return sizeof(builtin_cmds) / sizeof(builtin_cmd); }

int lsh_builtin_cd(char **args) {
  int rc;
  if ((rc = chdir(args[1])) == -1) {
    perror(NULL);
  }

  return 1;
}

int lsh_builtin_help(char **args) {
  fprintf(stderr, "Usage: command [option]\n");
  fprintf(stderr, "The cmd are built in:\n");
  for (int i = 0; i < lsh_builtin_cmd_num(); i++) {
    fprintf(stderr, "  %s\n", builtin_cmds[i].name);
  }
  return 1;
}

int lsh_builtin_exit(char **args) { return 0; }

int lsh_execute(char **args) {
  int status;
  int pid;
  pid = fork();

  if (pid == -1) {
    perror("fork error");
    exit(EXIT_FAILURE);
  }
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror(NULL);
    }
    exit(EXIT_FAILURE);
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}
