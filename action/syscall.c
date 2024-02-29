#include "syscall.h"

#include "common.h"
#include "config.h"
#include "file_trait.h"

#include "sys/uio.h"
#include "unistd.h"

int process_syscall(void *file_trait, struct ConfigMap *op) {
  char *call = CONFIG_TRAIT(CONFIG_TRAIT(op, as_map, "call"), as_string);
  if (strcmp(call, "sys_write") == 0) {
    return syscall_sys_write(file_trait, op);
  } else if (strcmp(call, "sys_writev") == 0) {
    return syscall_sys_writev(file_trait, op);
  } else {
    EPRINTF("Unknown syscall: %s\n", call);
    return -1;
  }
  return 0;
}

int syscall_sys_write(void *file_trait, struct ConfigMap *op) {
  struct FileTrait *ft = *(struct FileTrait **) file_trait;
  void *data = CONFIG_TRAIT(CONFIG_TRAIT(op, as_map, "data"), as_string);
  char *len_str = CONFIG_TRAIT(CONFIG_TRAIT(op, as_map, "len"), as_string);
  MUST(data != NULL, "data is null");
  MUST(len_str != NULL, "len is null");
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

static int syscall_sys_writev(void *file_trait, struct ConfigMap *op) {
  int iov_bufsize = 16;
  struct ConfigObj *iov_bufsize_obj = CONFIG_TRAIT(op, as_map, "iov-bufsize");
  if (iov_bufsize_obj != NULL) {
    CONFIG_REQUIRE(iov_bufsize_obj, CONFIG_TYPE_BYTE);
    char *iov_bufsize_str = CONFIG_TRAIT(iov_bufsize_obj, as_string);
    MUST_STR2UL(iov_bufsize, iov_bufsize_str);
    MUST(iov_bufsize > 0, "iov_bufsize is 0");
  }

  struct iovec *iov = malloc(sizeof(struct iovec) * iov_bufsize);
  size_t iov_cnt = 0;
  struct ConfigObj *data = CONFIG_TRAIT(op, as_map, "data");
  MUST(data != NULL, "data is null");
  CONFIG_REQUIRE(data, CONFIG_TYPE_ARRAY);

  size_t iov_cnt_in_cfg = CONFIG_TRAIT(data, as_array_size);
  size_t wish_to_write = 0;
  while (iov_cnt < iov_cnt_in_cfg) {
    struct ConfigObj *obj = CONFIG_TRAIT(data, as_array, iov_cnt);
    MUST(obj != NULL, "obj is null");
    CONFIG_REQUIRE(obj, CONFIG_TYPE_MAP);

    struct ConfigByte *buf = CONFIG_TRAIT(CONFIG_TRAIT(obj, as_map, "iov_base"), as_byte);
    MUST(buf != NULL, "buf is null");

    size_t len = buf->len;
    struct ConfigObj *iov_len_obj = CONFIG_TRAIT(obj, as_map, "iov_len");
    if (iov_len_obj != NULL) {
      CONFIG_REQUIRE(iov_len_obj, CONFIG_TYPE_BYTE);
      char *len_str = CONFIG_TRAIT(iov_len_obj, as_string);
      MUST_STR2UL(len, len_str);
    }
    iov[iov_cnt % iov_bufsize].iov_base = buf->data;
    iov[iov_cnt % iov_bufsize].iov_len = len;
    wish_to_write += len;
    iov_cnt++;
    if (iov_cnt % iov_bufsize == 0) {
      int fd = (*(struct FileTrait **) file_trait)->fd(file_trait);
      ssize_t ret = writev(fd, iov, iov_bufsize);
      if (ret == -1 || ret != wish_to_write) {
        EPRINTF("writev failed\n");
        return -1;
      }
      wish_to_write = 0;
    }
  }
  if (iov_cnt % iov_bufsize != 0) {
    int fd = (*(struct FileTrait **) file_trait)->fd(file_trait);
    ssize_t ret = writev(fd, iov, iov_cnt % iov_bufsize);
    if (ret == -1 || ret != wish_to_write) {
      EPRINTF("writev failed\n");
      return -1;
    }
  }

  free(iov);
  return 0;
}
