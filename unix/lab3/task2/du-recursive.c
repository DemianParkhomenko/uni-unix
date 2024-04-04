// gcc ./du-recursive.c -o du-recursive.out; ./du-recursive.out ./du-recursive.out
// du -bls ./du.out
// -b -> bytes -s -> --summarize (display only a total for each argument)

// The DIR data type represents a directory stream.

//https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html

// struct stat {
// dev_t     st_dev;     /* ID of device containing file */
// ino_t     st_ino;     /* inode number */
// mode_t    st_mode;    /* protection */
// nlink_t   st_nlink;   /* number of hard links */
// uid_t     st_uid;     /* user ID of owner */
// gid_t     st_gid;     /* group ID of owner */
// dev_t     st_rdev;    /* device ID (if special file) */
// off_t     st_size;    /* total size, in bytes */
// blksize_t st_blksize; /* blocksize for file system I/O */
// blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
// time_t    st_atime;   /* time of last access */
// time_t    st_mtime;   /* time of last modification */
// time_t    st_ctime;   /* time of last status change */
// }

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PATH 1024

const char* GREEN = "\033[0;32m";
const char* PURPLE = "\033[0;35m";
const char* RESET = "\033[0m";

struct du_args {
  size_t total_du;
  struct stat sb;
  size_t arg_total_du;
  bool vrb;
};

void du(char* path, struct du_args* da);

void add_size(char* path, struct du_args* da) {
  da->total_du += da->sb.st_size;
  da->arg_total_du += da->sb.st_size;
  if (da->vrb) printf("%s %zu \n", path, da->sb.st_size);
}

bool validate_dir_path(char* path, struct dirent* entry) {
  bool is_ok_length = MAX_PATH > strlen(path) + strlen(entry->d_name);
  if (is_ok_length == false) printf("Error: path too long\n");
  bool is_not_parent_or_current =
    strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0;
  return is_ok_length && is_not_parent_or_current;
}

void traverse_dir(char* path, struct dirent* entry, DIR* dir, struct du_args* da) {
  if ((entry = readdir(dir)) == NULL) return;
  bool is_valid_path = validate_dir_path(path, entry);
  if (is_valid_path) {
    char next_path[MAX_PATH];
    sprintf(next_path, "%s/%s", path, entry->d_name);
    du(next_path, da);
  }
  traverse_dir(path, entry, dir, da);
}

void du(char* path, struct du_args* da) {
  if (lstat(path, &(da->sb)) == -1) {
    printf("Error: could not get stats for %s\n", path);
    return;
  }
  if (S_ISREG(da->sb.st_mode) || S_ISLNK(da->sb.st_mode)) {
    add_size(path, da);
    return;
  }
  if (S_ISDIR(da->sb.st_mode)) {
    add_size(path, da);
    DIR* dir = opendir(path);
    if (dir == NULL) {
      printf("%s\n", strerror(errno));
      return;
    }
    struct dirent* entry;
    traverse_dir(path, entry, dir, da);
    closedir(dir);
  }
}

int main(int argc, char* argv[]) {
  struct du_args da = {
    .total_du = 0,
    .arg_total_du = 0,
    .vrb = false,
  };
  bool is_vrb_second_arg = argc == 2 && strcmp(argv[1], "-v") == 0;
  if (argc == 1 || is_vrb_second_arg) {
    if (is_vrb_second_arg) da.vrb = true;
    du(".", &da);
    printf("%sCurrent directory disk usage %zu %s\n",
      PURPLE, da.total_du, RESET); //0/0 %zu size_t 
  } else {
    for (int j = 1; j < argc; j++) {//separate loop if flags before paths
      if (strcmp(argv[j], "-v") == 0) {
        da.vrb = true;
        break;
      }
    }
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-') {
        du(argv[i], &da);
        printf("%sDisk usage for arg: %s %zu %s\n",
          GREEN, argv[i], da.arg_total_du, RESET);
        da.arg_total_du = 0;
      }
    }
    printf("%sTotal disk usage for all args: %zu %s\n",
      PURPLE, da.total_du, PURPLE);
  }
  return 0;
}
