#ifndef LINUX_FOPER_H
#define LINUX_FOPER_H

#if defined(__linux__)
int file_get_attr(int fd, unsigned long *f);
int file_ch_attr(int fd, unsigned long f);
int file_add_attr(int fd, unsigned long f);
int file_remove_attr(int fd, unsigned long f);
int file_has_attr(int fd, unsigned long f);
#endif

#endif
