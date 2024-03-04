#include "attr.h"

#include <errno.h>
#include <string.h>

#include <linux/fs.h>
#include <sys/ioctl.h>

#include "common.h"

int file_get_attr(int fd, unsigned long *f) {
  MUST_OR_RET(ioctl(fd, FS_IOC_GETFLAGS, f) == 0, -1, "ioctl failed: %s", strerror(errno));
  return 0;
}

int file_ch_attr(int fd, unsigned long f) {
  MUST_OR_RET(ioctl(fd, FS_IOC_SETFLAGS, &f) == 0, -1, "ioctl failed: %s", strerror(errno));
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

int file_set_attr(int fd, const char *attr, int old_fd) {
  unsigned long old_flag;
  file_get_attr(old_fd, &old_flag);

  unsigned long add_flag = 0;
  unsigned long remove_flag = 0;
  unsigned long set_flag = 0;
  char stat = 0;

  const char *p = attr;
  for (; *p != '\0'; ++p) {
    switch (*p) {
      case '+':
        stat = 1;
        continue;
      case '-':
        stat = 2;
        continue;
      case '=':
        stat = 3;
        continue;
    }

    if (stat == 0) {
      EPRINTF("invalid attr: %s", attr);
      return -1;
    }
    unsigned long flag;
    switch (*p) {
      case 'a':
        flag = FS_APPEND_FL;
        break;
      case 'A':
        flag = FS_NOATIME_FL;
        break;
      case 'c':
        flag = FS_COMPR_FL;
        break;
      case 'C':
        flag = FS_NOCOW_FL;
        break;
      case 'd':
        flag = FS_NODUMP_FL;
        break;
      case 'D':
        flag = FS_DIRSYNC_FL;
        break;
      case 'e':
        flag = FS_EXTENT_FL;
        break;
      case 'i':
        flag = FS_IMMUTABLE_FL;
        break;
      case 'j':
        flag = FS_JOURNAL_DATA_FL;
        break;
      case 'P':
        flag = FS_PROJINHERIT_FL;
        break;
      case 's':
        flag = FS_SECRM_FL;
        break;
      case 'S':
        flag = FS_SYNC_FL;
        break;
      case 't':
        flag = FS_NOTAIL_FL;
        break;
      case 'T':
        flag = FS_TOPDIR_FL;
        break;
      case 'u':
        flag = FS_UNRM_FL;
        break;
      default:
        EPRINTF("invalid attr: %s", attr);
        return -1;
    }

    switch (stat) {
      case 1:
        add_flag |= flag;
        break;
      case 2:
        remove_flag |= flag;
        break;
      case 3:
        set_flag |= flag;
        break;
    }
  }
  if (set_flag != 0 && (add_flag != 0 || remove_flag != 0)) {
    EPRINTF("invalid attr: %s", attr);
    return -1;
  }

  if (add_flag != 0) {
    MUST_OR_RET(file_add_attr(fd, add_flag) == 0, -1, "");
  }
  if (remove_flag != 0) {
    MUST_OR_RET(file_remove_attr(fd, remove_flag) == 0, -1, "");
  }
  if (set_flag != 0) {
    MUST_OR_RET(file_ch_attr(fd, set_flag) == 0, -1, "");
  }
  return 0;
}

int file_copy_attr(int dst, int src) {
  unsigned long flag;
  MUST_OR_RET(file_get_attr(src, &flag) == 0, -1, "");
  MUST_OR_RET(file_ch_attr(dst, flag) == 0, -1, "");
  return 0;
}
