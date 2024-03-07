#include "common.h"

#include <errno.h>
#include <string.h>

#include <sys/sendfile.h>
#include <sys/stat.h>

#include "../../common.h"
#include "file/trait.h"

int is_reg_file(const char *filename) {
  struct stat path_stat;
  if (stat(filename, &path_stat) != 0) {
    if (errno == ENOENT) {
      return 1;
    }
    MUST(0, "stat failed: %s", strerror(errno));
  }
  return S_ISREG(path_stat.st_mode);
}

int is_dir(const char *filename) {
  struct stat path_stat;
  if (stat(filename, &path_stat) != 0) {
    if (errno == ENOENT) {
      return 0;
    }
    MUST(0, "stat failed: %s", strerror(errno));
  }
  return S_ISDIR(path_stat.st_mode);
}

int copy(struct FileTrait **dst, struct FileTrait **src) {
  TRAIT(dst, FileTrait, flush);
  TRAIT(src, FileTrait, flush);

  int fd_dst = TRAIT(dst, FileTrait, fd);
  int fd_src = TRAIT(src, FileTrait, fd);

  struct stat st;
  fstat(fd_src, &st);

  ssize_t n = sendfile(fd_dst, fd_src, NULL, st.st_size);
  MUST_OR_RET(n >= 0, -1, "sendfile failed: %s", strerror(errno));
  MUST_OR_RET(n == st.st_size, -1, "sendfile failed: %s", strerror(errno));

  return 0;
}
