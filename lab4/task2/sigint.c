// https://www.gnu.org/software/libc/manual/html_node/Blocking-for-Handler.html
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <stddef.h>

bool isLooped = true;
int i = 0;

void sigint_handler() {
  i++;
}

void sigterm_handler() {
  isLooped = false;
}

int main(int argc, char** argv) {
  printf("To stop me:\nkill -s SIGTERM %d\n", getpid());

  struct sigaction act_sigint;
  act_sigint.sa_handler = sigint_handler;
  sigset_t block_sigterm;
  sigemptyset(&block_sigterm);
  sigaddset(&block_sigterm, SIGTERM);
  act_sigint.sa_mask = block_sigterm;
  sigaction(SIGINT, &act_sigint, NULL);

  struct sigaction act_sigterm;
  act_sigterm.sa_handler = sigterm_handler;
  sigset_t block_sigint;
  sigemptyset(&block_sigint);
  sigaddset(&block_sigint, SIGINT);
  act_sigterm.sa_mask = block_sigint;
  sigaction(SIGTERM, &act_sigterm, NULL);

  while (isLooped);

  printf("\nHandled SIGINT %d times\n", i);
  return 0;
}

