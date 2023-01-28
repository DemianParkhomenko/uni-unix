#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
//gcc ./system.c -o system.out; ./system.out tree

int main(int argc, char const* argv[]) {
  if (argc < 2) {
    printf("Usage %s cmd_string\n", argv[0]);
    exit(1);
  }
  int STR_LENGTH = 1024;
  char cmd_string[STR_LENGTH];
  cmd_string[0] = '\0';
  char buf[STR_LENGTH];
  buf[0] = '\0'; // both arguments of the strcat function call must point to strings
  for (int i = 1; i < argc; i++) {
    if (strlen(cmd_string) + 1 + strlen(argv[i]) > STR_LENGTH) {
      printf("cmd_string is too long\n");
      exit(1);
    }
    sprintf(buf, "%s ", argv[i]);
    strcat(cmd_string, buf);
  }
  int sys_res = system(cmd_string);
  switch (sys_res) {
  case -1: printf("fork failed\n"); break;
  case 127: printf("exec failed\n"); break;
  default: printf("Child process code %d\n", sys_res);
  }
  return 0;
}
