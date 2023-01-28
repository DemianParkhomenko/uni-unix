// gcc ./du-loop.c -o du-loop.out; ./du-loop.out
// https://github.com/coreutils/coreutils/blob/master/src/du.c
//! to compare with du: use du -bls 
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_PATH 1024

const char* GREEN = "\033[0;32m";
const char* PURPLE = "\033[0;35m";
const char* RESET = "\033[0m";

void print_du(char* path, size_t size, bool vrb) {
  if (vrb)
    printf("%s %zu\n", path, size);
}

void du(char* path, size_t* p_usage, bool vrb) {
  DIR* dir;
  struct dirent* entry;
  char next_path[MAX_PATH];
  struct stat sb;

  if (lstat(path, &sb) == -1) {
    fprintf(stderr, "Could not get stats for %s: %s\n",
      path, strerror(errno));
    return;
  }
  if (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode)) {
    *p_usage += sb.st_size;
    print_du(path, sb.st_size, vrb);
    return;
  }
  if (S_ISDIR(sb.st_mode)) {
    if (!(dir = opendir(path))) {
      fprintf(stderr, "Could not open directory %s: %s\n",
        path, strerror(errno));
      return;
    }
    *p_usage += sb.st_size;
    print_du(path, sb.st_size, vrb);
    while (entry = readdir(dir)) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;
      if (MAX_PATH < strlen(path) + strlen(entry->d_name) + 2) {
        fprintf(stderr, "Path %s/%s is too long. Max path length is %d\n",
          path, entry->d_name, MAX_PATH);
        continue;
      }
      sprintf(next_path, "%s/%s", path, entry->d_name);
      du(next_path, p_usage, vrb);
    }
    closedir(dir);
  }
}

int main(int argc, char* argv[]) {
  size_t usage = 0;
  size_t total_usage = 0;
  bool vrb = false;

  for (int i = 1; i < argc; i++)
    if (strcmp(argv[i], "-v") == 0)
      vrb = true;

  if (argc == 1 || (argc == 2 && vrb)) {
    du(".", &usage, vrb);
    printf("%sCurrent directory disk usage %zu %s\n",
      PURPLE, usage, RESET);
  } else {
    for (int j = 1; j < argc; j++) {
      if (argv[j][0] != '-') {
        du(argv[j], &usage, vrb);
        printf("%sDisk usage for arg: %s %zu %s\n",
          GREEN, argv[j], usage, RESET);
        total_usage += usage;
        usage = 0;
      }
    }
    printf("%sTotal disk usage: %zu %s\n",
      PURPLE, total_usage, PURPLE);
  }
  return 0;
}
