#include "dir.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>

#include "common.h"

static int inner_visit_dir(const char *dirname, const char *step_path, int (*callback)(struct dirent *entry, const char *ori_path, const char *step_path, void *arg), void *arg) {
  FN_META();

  DIR *dir;
  struct dirent *entry;
  char *new_step_path;
  int ret;

  char *to_open = malloc(strlen(dirname) + strlen(step_path) + 2);
  MUST_OR_FREE_RET(to_open != NULL, -1, "malloc failed");
  free_list_add(to_open, free_adapter);
  sprintf(to_open, "%s%s%s", dirname, step_path[0] == '\0' ? "" : "/", step_path);

  dir = opendir(to_open);
  MUST_OR_FREE_RET(dir != NULL, -1, "opendir failed: %s: %s", strerror(errno), to_open);
  free_list_add(dir, (int (*)(void *)) closedir);

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    int cb_ret = callback(entry, dirname, step_path, arg);
    MUST_OR_FREE_RET(cb_ret != -1, -1, "callback failed: to_open: %s", to_open);

    if (entry->d_type == DT_DIR && cb_ret != 1) {
      new_step_path = malloc(strlen(step_path) + strlen(entry->d_name) + 2);
      MUST_OR_FREE_RET(new_step_path != NULL, -1, "malloc failed");
      sprintf(new_step_path, "%s%s%s", step_path, step_path[0] == '\0' ? "" : "/", entry->d_name);
      ret = inner_visit_dir(dirname, new_step_path, callback, arg);
      free(new_step_path);
      MUST_OR_FREE_RET(ret == 0, -1, "inner_visit_dir failed: %s", strerror(errno));
    }
  }

  FN_CLEAN();
  return 0;
}

int visit_dir(const char *dirname, int (*callback)(struct dirent *entry, const char *ori_path, const char *step_path, void *arg), void *arg) {
  return inner_visit_dir(dirname, "", callback, arg);
}
