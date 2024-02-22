#ifndef STD_FILE_WRAPPER_H
#define STD_FILE_WRAPPER_H

#include <stdio.h>

#include "file_interface.h"

int std_read(void *self, void *buf, size_t len);
int std_write(void *self, const void *buf, size_t len);
int std_seek(void *self, ssize_t offset, int whence);
int std_truncate(void *self, size_t len);
int std_close(void *self);

extern struct FileTrait std_file_trait;

struct StdFile {
  struct FileTrait read_write_seeker;
  FILE *file;
};

void *std_file_init(const char *filename, const char *mode);

#endif
