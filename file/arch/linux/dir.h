#ifndef LINUX_DIR_H
#define LINUX_DIR_H

#include <dirent.h>

int visit_dir(const char *dir_name, int (*callback)(struct dirent *entry, const char *ori_path, const char *step_path, void *arg), void *arg);

#endif
