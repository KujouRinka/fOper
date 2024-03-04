#ifndef FILE_LINUX_XATTR_H
#define FILE_LINUX_XATTR_H

#include <stdlib.h>

int file_list_xattr(int fd, char **list, ssize_t *length);
int file_get_xattr(int fd, const char *name, char **value, ssize_t *length);
int file_set_xattr(int fd, const char *name, const char *value);
int file_remove_xattr(int fd, const char *name);
int file_has_xattr(int fd, const char *name, const char *value);

int file_copy_xattr(int dst, int src);

int file_log_xattr(int fd);

#endif
