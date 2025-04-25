#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char **items;
  size_t size;
  size_t capacity;
} da_str_t;

#define da_append(da, item)                                                    \
  do {                                                                         \
    if ((da)->size >= (da)->capacity) {                                        \
      if ((da)->capacity == 0) {                                               \
        (da)->capacity = 256;                                                  \
      } else {                                                                 \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items =                                                            \
          realloc((da)->items, (da)->capacity * sizeof(*(da)->items));         \
    }                                                                          \
    (da)->items[(da)->size++] = item;                                          \
  } while (0)

#define da_foreach(Type, it, da)                                               \
  for (Type *it = (da)->items; it < (da)->items + (da)->size; ++it)

#define da_free(da) free((da)->items)

#ifdef DEBUG
#define log_infoln(fmt, ...) printf("INFO: " fmt "\n", ##__VA_ARGS__)
#else // DEBUG
#define log_infoln(fmt, ...) ((void)0)
#endif // DEBUG

// list all files and directories at a given path
da_str_t ls(const char *path) {
  DIR *dp = opendir(path);
  if (dp == NULL) {
    if (errno == ENOTDIR) {
      return (da_str_t){0};
    }
    perror("opendir");
    exit(EXIT_FAILURE);
  }

  da_str_t files = {0};
  struct dirent *entry;
  while ((entry = readdir(dp))) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    da_append(&files, strdup(entry->d_name));
  }

  closedir(dp);

  return files;
}

char *concat(const char *s1, const char *s2) {
  char *result = malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

da_str_t find_git(const char *path) {
  log_infoln("current: %s", path);

  da_str_t next = ls(path);
  if (next.size == 0) {
    log_infoln("\tfile or empty directory");
    return (da_str_t){0};
  }

  bool is_repo = false;
  da_foreach(char *, f, &next) {
    log_infoln("\t\tnext: %s", *f);
    if (strcmp(*f, ".git") == 0) {
      is_repo = true;
    } else if (strcmp(*f, "ORIG_HEAD") == 0) {
      is_repo = true;
    }
  }

  da_str_t repos = {0};

  if (is_repo) {
    log_infoln("\trepo");
    da_append(&repos, (char *)path);
  } else {
    log_infoln("\tNOT a repo");
    da_foreach(char *, f, &next) {
      char *foo = concat(path, "/");
      char *bar = concat(foo, *f);

      da_str_t res = find_git(bar);
      da_foreach(char *, r, &res) { da_append(&repos, *r); }
      da_free(&res);

      free(bar);
      free(foo);
    }
  }

  da_foreach(char *, f, &next) { free(*f); }
  da_free(&next);

  return repos;
}

int main(int argc, char *argv[]) {
  da_str_t repos = find_git((argc > 1) ? argv[1] : ".");
  da_foreach(char *, r, &repos) { printf("%s\n", *r); }

  da_free(&repos);

  return EXIT_SUCCESS;
}
