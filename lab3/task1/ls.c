// gcc ls.c -o ls.out; ./ls.out -v
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define SEP "\033[0;33m|\033[0m"
#define FORMAT(explanation, code) {\
if (vrb) { \
printf(SEP);\
printf("%s", explanation);\
}\
code;\
if (vrb) printf("\n");\
}

void pr_mode(mode_t mode, bool vrb) {
  switch (mode & S_IFMT) {
  case S_IFREG: vrb ? printf("regular ") : printf("-"); break;
  case S_IFDIR: vrb ? printf("directory ") : printf("d"); break;
  case S_IFCHR: vrb ? printf("character device ") : printf("c"); break;
  case S_IFBLK: vrb ? printf("block device ") : printf("b"); break;
  case S_IFLNK: vrb ? printf("symbolic (soft) link ") : printf("l"); break;
  case S_IFSOCK: vrb ? printf("socket ") : printf("s"); break;
  case S_IFIFO: vrb ? printf("FIFO/pipe ") : printf("p"); break;
  default: printf("unknown file type ");
  }
  printf("%c%c%c%c%c%c%c%c%c ",
    (mode & S_IRUSR) ? 'r' : '-',
    (mode & S_IWUSR) ? 'w' : '-',
    (mode & S_IXUSR) ? 'x' : '-',
    (mode & S_IRGRP) ? 'r' : '-',
    (mode & S_IWGRP) ? 'w' : '-',
    (mode & S_IXGRP) ? 'x' : '-',
    (mode & S_IROTH) ? 'r' : '-',
    (mode & S_IWOTH) ? 'w' : '-',
    (mode & S_IXOTH) ? 'x' : '-'
  );
}

int main(int argc, char const* argv[]) {
  const char* path = ".";
  const int num_flags = 1;
  const int max_args = 2;
  DIR* dir;

  bool vrb = false;
  if (argc > max_args + num_flags) {
    printf("Usage: %s <directory>\n\t -v\tverbose info\n", argv[0]);
    return 0;
  }
  for (size_t i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      vrb = true;
      continue;
    }
    path = argv[i];
  }

  if (!(dir = opendir(path))) {
    fprintf(stderr, "Could not open a directory %s: %s\n", path, strerror(errno));
    exit(1);
  }

  struct dirent* entry;
  struct stat sb;
  char entry_path[1024];
  while (entry = readdir(dir)) {
    sprintf(entry_path, "%s/%s", path, entry->d_name);
    if (lstat(entry_path, &sb) == -1) {
      fprintf(stderr, "Error: could not get stats for %s: %s \n", entry->d_name, strerror(errno));
      continue;
    }
    FORMAT("", pr_mode(sb.st_mode, vrb));
    FORMAT("hard links: ", printf("%d ", sb.st_nlink));
    struct passwd* pw = getpwuid(sb.st_uid);
    FORMAT("user: ", printf("%s ", (pw == NULL) ? "unknown " : pw->pw_name));
    struct group* gr = getgrgid(sb.st_gid);
    FORMAT("group: ", printf("%s ", (gr == NULL) ? "unknown " : gr->gr_name));
    FORMAT("size in bytes: ", printf("%ld ", sb.st_size));
    FORMAT("modify time: ", printf("%s ", strtok(ctime(&sb.st_mtime), "\n")));
    FORMAT("inode number: ", printf("%ld ", sb.st_ino));
    FORMAT("file name: ", printf("%s\n", entry->d_name));
  }
  closedir(dir);
  return 0;
}
