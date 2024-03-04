#ifndef ACTION_COPY_FILE_PROPERTY_H
#define ACTION_COPY_FILE_PROPERTY_H

struct FileTrait;
struct ConfigObj;

int write_property(struct FileTrait *f_dst, struct FileTrait *f_src, struct ConfigObj *config);
int fwrite_property(int fd_dst, int fd_src, struct ConfigObj *config);
int fnwrite_property(const char *dst, const char *src, struct ConfigObj *config);

int fnwrite_dir_property(const char *dst, const char *src, struct ConfigObj *config);

#endif
