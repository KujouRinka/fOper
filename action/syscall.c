#include "syscall.h"

#include "config.h"
#include "file_trait.h"
#include "common.h"

#include "unistd.h"

int process_syscall(void *file_trait, struct Operation *op) {
  char *call = OperationGet(op, "call");
  if (strcmp(call, "sys_write") == 0) {
    return syscall_sys_write(file_trait, op);
  } else {
    EPRINTF("Unknown syscall: %s\n", call);
    return -1;
  }
  return 0;
}

int syscall_sys_write(void *file_trait, struct Operation *op) {
  struct FileTrait *ft = *(struct FileTrait **) file_trait;
  void *data = OperationGet(op, "data");
  char *len_str = OperationGet(op, "len");
  ssize_t len;
  MUST_STR2L(len, len_str);

  int fd = ft->fd(file_trait);
  while (len > 0) {
    ssize_t ret = write(fd, data, len);
    if (ret < 0) {
      EPRINTF("write failed\n");
      return -1;
    }
    len -= ret;
  }
  return 0;
}
