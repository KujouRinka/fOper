#include "std_file_wrapper.h"

#include "common.h"

struct FileTrait std_file_trait = {
    .read = std_read,
    .write = std_write,
    .seek = std_seek,
    .truncate = std_truncate,
    .flush = std_flush,
    .fd = std_fd,
    .filename = std_filename,
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
  MUST_OR_RET(ret == len, -1, "fwrite failed");

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

int std_flush(void *self) {
  struct StdFile *f = (struct StdFile *) self;
  return fflush(f->file);
}

int std_fd(void *self) {
  struct StdFile *f = (struct StdFile *) self;
  return fileno(f->file);
}

char *std_filename(void *self) {
  struct StdFile *f = (struct StdFile *) self;
  return f->filename;
}

int std_close(void *self) {
  struct StdFile *f = (struct StdFile *) self;
  fclose(f->file);
  free(f->filename);
  free(f);
  return 0;
}

struct FileTrait **std_file_init(const char *filename, const char *mode) {
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
  file->filename = strdup(filename);
  return (struct FileTrait **) file;
}
