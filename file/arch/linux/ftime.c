#include "ftime.h"

#include <errno.h>
#include <stdio.h>
#include <time.h>

#include <sys/stat.h>
#include <utime.h>

#include "common.h"
#include "strptime.h"

static time_t convert_to_timestamp(const char *datetime, const char *fmt) {
  struct tm tm;
  memset(&tm, 0, sizeof(struct tm));

  MUST_OR_RET(strptime(datetime, fmt, &tm) != NULL, -1, "strptime failed");

  return mktime(&tm);
}

int file_set_ctime(const char *filename, const char *time, const char *fmt) {
  UNIMPLEMENTED();
  return 0;
}

int file_set_mtime(const char *filename, const char *time, const char *fmt) {
  struct stat st;
  MUST_OR_RET(stat(filename, &st) == 0, -1, "stat failed");

  time_t timestamp = convert_to_timestamp(time, fmt);
  MUST_OR_RET(timestamp != -1, -1, "convert_to_timestamp failed");

  struct utimbuf times;
  // keep actime unchanged
  times.actime = st.st_atime;
  times.modtime = timestamp;

  MUST_OR_RET(utime(filename, &times) == 0 || errno != EPERM, -1, "utime failed: %s", strerror(errno));
  return 0;
}

int file_set_atime(const char *filename, const char *time, const char *fmt) {
  struct stat st;
  MUST_OR_RET(stat(filename, &st) == 0, -1, "stat failed: %s", strerror(errno));

  time_t timestamp = convert_to_timestamp(time, fmt);
  MUST_OR_RET(timestamp != -1, -1, "convert_to_timestamp failed: %s", strerror(errno));

  struct utimbuf times;
  times.actime = timestamp;
  // keep modtime unchanged
  times.modtime = st.st_mtime;

  MUST_OR_RET(utime(filename, &times) == 0 || errno != EPERM, -1, "utime failed: %s", strerror(errno));
  return 0;
}

int file_copy_ctime(const char *dst, const char *src) {
  UNIMPLEMENTED();
  return 0;
}

int file_copy_mtime(const char *dst, const char *src) {
  struct stat st_dst, st_src;
  MUST_OR_RET(stat(src, &st_src) == 0, -1, "stat failed: %s", strerror(errno));
  MUST_OR_RET(stat(dst, &st_dst) == 0, -1, "stat failed: %s", strerror(errno));

  struct utimbuf times;
  // keep actime unchanged
  times.actime = st_dst.st_atime;
  times.modtime = st_src.st_mtime;

  MUST_OR_RET(utime(dst, &times) == 0 || errno != EPERM, -1, "utime failed: %s", strerror(errno));
  return 0;
}

int file_copy_atime(const char *dst, const char *src) {
  struct stat st_dst, st_src;
  MUST_OR_RET(stat(src, &st_src) == 0, -1, "stat failed: %s", strerror(errno));
  MUST_OR_RET(stat(dst, &st_dst) == 0, -1, "stat failed: %s", strerror(errno));

  struct utimbuf times;
  times.actime = st_src.st_atime;
  // keep modtime unchanged
  times.modtime = st_dst.st_mtime;

  MUST_OR_RET(utime(dst, &times) == 0 || errno != EPERM, -1, "utime failed: %s", strerror(errno));

  return 0;
}

int file_copy_times(const char *dst, const char *src) {
  struct stat st_dst, st_src;
  MUST_OR_RET(stat(src, &st_src) == 0, -1, "stat failed: %s", strerror(errno));
  MUST_OR_RET(stat(dst, &st_dst) == 0, -1, "stat failed: %s", strerror(errno));

  struct utimbuf times;
  times.actime = st_src.st_atime;
  times.modtime = st_src.st_mtime;

  MUST_OR_RET(utime(dst, &times) == 0 || errno != EPERM, -1, "utime failed: %s", strerror(errno));

  return 0;
}
