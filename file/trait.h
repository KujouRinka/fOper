#ifndef FILE_TRAIT_H
#define FILE_TRAIT_H

#include <stddef.h>

struct FileTrait {
  /// read upper to `size` bytes to `buf`
  int (*read)(void *self, void *buf, size_t len);
  /// write just `size` bytes from `buf`, or return -1 if failed
  int (*write)(void *self, const void *buf, size_t len);
  int (*seek)(void *self, ssize_t offset, int whence);
  int (*truncate)(void *self, size_t len);
  int (*flush)(void *self);
  int (*fd)(void *self);
  int (*close)(void *self);
  // TODO: more methods
};

#endif
