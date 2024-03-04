#ifndef COMMON_LINUX_H
#define COMMON_LINUX_H

struct FileTrait;

int is_reg_file(const char *filename);
int is_dir(const char *filename);
int copy(struct FileTrait *dst, struct FileTrait *src);

#endif
