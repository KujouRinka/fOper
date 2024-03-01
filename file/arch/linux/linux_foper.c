#include "linux_foper.h"

#include <linux/fs.h>
#include <sys/ioctl.h>

#include "common.h"

int file_get_attr(int fd, unsigned long *f) {
  MUST_OR_RET(ioctl(fd, FS_IOC_GETFLAGS, f) == 0, -1, "ioctl failed");
  return 0;
}

int file_ch_attr(int fd, unsigned long f) {
  MUST_OR_RET(ioctl(fd, FS_IOC_SETFLAGS, &f) == 0, -1, "ioctl failed");
  return 0;
}

int file_add_attr(int fd, unsigned long f) {
  unsigned long flag;
  MUST_OR_RET(file_get_attr(fd, &flag) == 0, -1, "");
  flag |= f;
  MUST_OR_RET(file_ch_attr(fd, flag) == 0, -1, "");
  return 0;
}

int file_remove_attr(int fd, unsigned long f) {
  unsigned long flag;
  MUST_OR_RET(file_get_attr(fd, &flag) == 0, -1, "");
  flag &= ~(f);
  MUST_OR_RET(file_ch_attr(fd, flag) == 0, -1, "");
  return 0;
}

int file_has_attr(int fd, unsigned long f) {
  unsigned long flag;
  MUST_OR_RET(file_get_attr(fd, &flag) == 0, -1, "file_get_attr failed");
  return (flag & f) == f ? 1 : 0;
}
