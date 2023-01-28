#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>

#define MIN_CHILDREN 1
#define MAX_CHILDREN 10
#define SLEEP_S 3

int num_children;
int log_fd;

void child_sigusr1_handler() {
  char log_message[1024];
  time_t t;
  time(&t);
  sprintf(log_message, "%s Child %d got signal from parent\n", strtok(ctime(&t), "\n"), getpid());
  write(log_fd, log_message, strlen(log_message));
  kill(getppid(), SIGUSR2);
  sleep(SLEEP_S);
}

void parent_sigusr2_handler(int signo, siginfo_t* info, void* context) {
  static int received_child_signal_pids[MAX_CHILDREN];
  char log_message[1024];
  time_t t;
  time(&t);
  if (received_child_signal_pids[num_children - 1] != 0) {
    memset(received_child_signal_pids, 0, sizeof received_child_signal_pids);
    sleep(SLEEP_S);
    sprintf(log_message, "Parent had slept\n");
  } else {
    for (int i = 0; i < MAX_CHILDREN; i++) {
      if (received_child_signal_pids[i] == info->si_pid) {
        //? sprintf("Duplicate signal from pid %d\n", info->si_pid);
        return;//break;
      }
      if (received_child_signal_pids[i] == 0) {
        received_child_signal_pids[i] = info->si_pid;
        sprintf(log_message,
          "%s Parent %ld got SIGUSR2 signal from child %d\n",
          strtok(ctime(&t), "\n"),
          getpid(),
          info->si_pid);
        break;
      }
    }
  }
  write(log_fd, log_message, strlen(log_message));
}

void parent_sigterm_handler() {
  char log_message[1024];
  time_t t;
  time(&t);
  sprintf(log_message, "%s Parent %d got SIGTERM\n", strtok(ctime(&t), "\n"), getpid());
  write(log_fd, log_message, strlen(log_message));

  killpg(getpid(), SIGTERM);
  pid_t wpid;
  while ((wpid = wait(NULL)) > 0);

  close(log_fd);
  exit(0);
}


void child_sigterm_handler() {
  char log_message[1024];
  time_t t;
  time(&t);
  sprintf(log_message, "%s Child %d got SIGTERM\n", strtok(ctime(&t), "\n"), getpid());
  write(log_fd, log_message, strlen(log_message));
  exit(0);
}

int main(int argc, char const* argv[]) {
  if (argc != 2) {
    printf("Usage %s number_of_child_process\n", argv[0]);
    exit(1);
  }

  num_children = atoi(argv[1]);
  if (MIN_CHILDREN > num_children || num_children > MAX_CHILDREN) {
    printf("number_of_child_process is out of the range: [%d, %d]\n",
      MIN_CHILDREN, MAX_CHILDREN);
    exit(1);
  }

  printf("To stop me:\nkill -s SIGTERM %d\n", getpid());

  log_fd = open("./log.txt",
    O_CREAT | O_RDWR | O_APPEND | O_TRUNC,
    S_IRUSR | S_IWUSR);

  if (log_fd == -1) {
    printf("Cannot open log file\n");
    exit(1);
  }

  struct sigaction act_ignore;
  act_ignore.sa_handler = SIG_IGN;
  sigemptyset(&act_ignore.sa_mask);
  sigaction(SIGUSR1, &act_ignore, NULL);

  int pid;
  for (int i = 0; i < num_children; i++) {
    pid = fork();
    if (pid == 0) {
      struct sigaction act;
      act.sa_handler = child_sigusr1_handler;
      sigemptyset(&act.sa_mask);
      sigaction(SIGUSR1, &act, NULL);
      setpgid(getpid(), getppid());
      break;
    }
  }

  if (pid > 0) {
    struct sigaction act_sigterm_parent;
    act_sigterm_parent.sa_sigaction = parent_sigterm_handler;
    sigemptyset(&act_sigterm_parent.sa_mask);
    sigaction(SIGTERM, &act_sigterm_parent, NULL);

    struct sigaction act_sigusr2;
    act_sigusr2.sa_sigaction = parent_sigusr2_handler;
    sigemptyset(&act_sigusr2.sa_mask);
    act_sigusr2.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &act_sigusr2, NULL);


    while (true)
      killpg(getpid(), SIGUSR1);
  } else {
    struct sigaction act_sigterm_child;
    act_sigterm_child.sa_sigaction = child_sigterm_handler;
    sigemptyset(&act_sigterm_child.sa_mask);
    sigaction(SIGTERM, &act_sigterm_child, NULL);
    while (true);
  }
  return 0;
}
