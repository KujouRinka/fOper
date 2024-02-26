#include "std_file_wrapper.h"

#include "common.h"

struct FileTrait std_file_trait = {
    .read = std_read,
    .write = std_write,
    .seek = std_seek,
    .truncate = std_truncate,
    .fd = std_fd,
    .close = std_close,
};

int std_read(void *self, void *buf, size_t len) {
  struct StdFile *f = (struct StdFile *) self;
  size_t ret = fread(buf, 1, len, f->file);
  if (ret < len) {
    if (feof(f->file)) {
      return 0;
    }
    return -1;
  }
  return 0;
}

int std_write(void *self, const void *buf, size_t len) {
  struct StdFile *f = (struct StdFile *) self;
  size_t ret = fwrite(buf, 1, len, f->file);
  if (ret < len) {
    return -1;
  }
  return 0;
}

int std_seek(void *self, ssize_t offset, int whence) {
  struct StdFile *f = (struct StdFile *) self;
  return fseek(f->file, offset, whence);
}

int std_truncate(void *self, size_t len) {
  struct StdFile *f = (struct StdFile *) self;
  UNIMPLEMENTED();
  return 0;
}

int std_fd(void *self) {
  struct StdFile *f = (struct StdFile *) self;
  return fileno(f->file);
}

int std_close(void *self) {
  struct StdFile *f = (struct StdFile *) self;
  fclose(f->file);
  return 0;
}

void *std_file_init(const char *filename, const char *mode) {
  struct StdFile *file = malloc(sizeof(struct StdFile));
  if (file == NULL) {
    return NULL;
  }
  file->file_trait = &std_file_trait;
  file->file = fopen(filename, mode);
  if (file->file == NULL) {
    free(file);
    return NULL;
  }
  return file;
}
