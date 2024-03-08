#ifndef FILE_LINUX_FTIME_H
#define FILE_LINUX_FTIME_H

#include <time.h>

int file_set_ctime(const char *filename, const char *time, const char *fmt);
int file_set_mtime(const char *filename, const char *time, const char *fmt);
int file_set_atime(const char *filename, const char *time, const char *fmt);

int file_copy_ctime(const char *dst, const char *src);
int file_copy_mtime(const char *dst, const char *src);
int file_copy_atime(const char *dst, const char *src);

int file_copy_times(const char *dst, const char *src);

#endif
