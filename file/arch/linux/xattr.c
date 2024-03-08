#include "xattr.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <sys/xattr.h>

#include "common.h"

int file_list_xattr(int fd, char **list, ssize_t *length) {
  MUST_OR_RET((*length = flistxattr(fd, NULL, 0)) >= 0, -1, "flistxattr failed: %s", strerror(errno));
  if (*length == 0) {
    *list = NULL;
    return 0;
  }

  MUST_OR_RET((*list = malloc(*length)) != NULL, -1, "malloc failed");

  MUST_OR_RET((*length = flistxattr(fd, *list, *length)) >= 0, -1, "flistxattr failed: %s", strerror(errno));

  return 0;
}

int file_get_xattr(int fd, const char *name, char **value, ssize_t *length) {
  MUST_OR_RET((*length = fgetxattr(fd, name, NULL, 0)) >= 0, -1, "fgetxattr failed: %s", strerror(errno));
  if (*length == 0) {
    *value = NULL;
    *length = 0;
    return 0;
  }

  MUST_OR_RET((*value = malloc(*length)) != NULL, -1, "malloc failed: %s, %d", strerror(errno), *length);

  MUST_OR_RET((*length = fgetxattr(fd, name, *value, *length)) >= 0, -1, "fgetxattr failed: %s", strerror(errno));
  return 0;
}

int file_set_xattr(int fd, const char *name, const char *value) {
  MUST_OR_RET(fsetxattr(fd, name, value, strlen(value), 0) == 0, -1, "fsetxattr failed: %s", strerror(errno));
  return 0;
}

int file_remove_xattr(int fd, const char *name) {
  MUST_OR_RET(fremovexattr(fd, name) == 0 || errno == ENODATA, -1, "fremovexattr failed: %s", strerror(errno));
  return 0;
}

int file_has_xattr(int fd, const char *name, const char *value) {
  char *actual_value = NULL;
  ssize_t length = 0;
  int ret = file_get_xattr(fd, name, &actual_value, &length);
  if (ret != 0) {
    return 0;
  }

  if (length != strlen(value) || memcmp(actual_value, value, length) != 0) {
    free(actual_value);
    return 0;
  }

  free(actual_value);
  return 1;
}

int file_copy_xattr(int dst, int src) {
  char *list = NULL;
  ssize_t length = 0;
  int ret = file_list_xattr(src, &list, &length);
  if (ret != 0) {
    return ret;
  }

  for (ssize_t i = 0; i < length; i += strlen(list + i) + 1) {
    char *value = NULL;
    ssize_t value_length = 0;
    ret = file_get_xattr(src, list + i, &value, &value_length);
    if (ret != 0) {
      free(list);
      return ret;
    }

    ret = file_set_xattr(dst, list + i, value);
    free(value);
    if (ret != 0) {
      free(list);
      return ret;
    }
  }

  free(list);
  return 0;
}

int file_log_xattr(int fd) {
  char *list = NULL;
  ssize_t length = 0;
  int ret = file_list_xattr(fd, &list, &length);
  if (ret != 0) {
    return ret;
  }

  for (ssize_t i = 0; i < length; i += strlen(list + i) + 1) {
    char *value = NULL;
    ssize_t value_length = 0;
    ret = file_get_xattr(fd, list + i, &value, &value_length);
    if (ret != 0) {
      free(list);
      return ret;
    }

    LOG("\033[33mxattr: %s=%.*s\033[0m", list + i, (int) value_length, value);
    free(value);
  }

  free(list);
  return 0;
}
