#ifndef FILE_LINUX_ATTR_H
#define FILE_LINUX_ATTR_H

int file_get_attr(int fd, unsigned long *f);
int file_ch_attr(int fd, unsigned long f);
int file_add_attr(int fd, unsigned long f);
int file_remove_attr(int fd, unsigned long f);
int file_has_attr(int fd, unsigned long f);

int file_set_attr(int fd, const char *attr, int old_fd);

int file_copy_attr(int dst, int src);

#endif
