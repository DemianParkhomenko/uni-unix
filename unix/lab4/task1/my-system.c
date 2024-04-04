#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define STR_LENGTH 1024
#define EXEC_ERR_CODE 127
#define FORK_ERR_CODE -1
#define WAITPID_ERR_CODE 256

int my_system(const char* cmd) {
  pid_t pid;
  if (cmd == NULL) {
    fprintf(stderr, "my_system: cmd is NULL\n");
    return 1;
  }
  if ((pid = fork()) < 0) {
    fprintf(stderr, "my_system: Fail on fork: %s\n", strerror(errno));
    return FORK_ERR_CODE;
  }
  if (pid == 0) {
    // -c mean cmd is a string
    execl("/bin/sh", "sh", "-c", cmd);// why /bin/sh sh 
    exit(EXEC_ERR_CODE);
  } else {
    int status;
    while (waitpid(pid, &status, 0) < 0) {
      if (errno != EINTR) {
        status = WAITPID_ERR_CODE;
      }
    }
    return status;
  }
}

int main(int argc, char const* argv[]) {
  char cmd[STR_LENGTH] = "";
  if (argc < 2) {
    printf("Usage %s shell_commands\n", argv[0]);
    exit(1);
  }
  for (int i = 1; i < argc; i++) {
    if (strlen(cmd) + strlen(argv[i] + 1) > STR_LENGTH) {
      printf("cmd is too long\n");
      exit(1);
    }
    sprintf(cmd + strlen(cmd), "%s ", argv[i]);
  }
  printf("Child process return code: %d\n", my_system(cmd));
  exit(0);
}
