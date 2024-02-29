#include "move.h"

#if defined(__linux__)
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "common.h"
#include "config.h"
#include "file_trait.h"

int action_move(void *file_trait, struct ConfigObj *config) {
  int fd = TRAIT(file_trait, FileTrait, fd);
  struct ConfigObj *byte_obj;

  byte_obj = TRAIT(config, ConfigObjTrait, as_map, "fill");
  int fill = TRAIT(byte_obj, ConfigObjTrait, as_int);

  byte_obj = TRAIT(config, ConfigObjTrait, as_map, "offset");
  int off = TRAIT(byte_obj, ConfigObjTrait, as_int);

  byte_obj = TRAIT(config, ConfigObjTrait, as_map, "len");
  int len = TRAIT(byte_obj, ConfigObjTrait, as_int);

  // if is linux and have posix_fallocate syscall, use it
  // if not, use seek and write trait
#if defined(__linux__)
  TRAIT(file_trait, FileTrait, flush);
  long file_size = lseek(fd, 0, SEEK_END);
  MUST_OR_RET(file_size >= 0, -1, "lseek failed");
  long where = lseek(fd, 0, SEEK_CUR);
  MUST_OR_RET(where >= 0, -1, "lseek failed");
  long to_move = file_size - off;
  int ret = posix_fallocate(fd, off, to_move + len);
  MUST_OR_RET(ret == 0, -1, "posix_fallocate failed");

  // reversely copy file from `off` to `off + len` with length `to_move`
  char buf[1024];
  size_t to_read = to_move > sizeof(buf) ? sizeof(buf) : to_move;
  size_t read_pos = off + to_move - to_read;
  size_t write_pos = read_pos + len;
  while (to_move > 0) {
    to_read = to_move > sizeof(buf) ? sizeof(buf) : to_move;
    size_t read = pread(fd, buf, to_read, read_pos);
    MUST_OR_RET(read > 0, -1, "pread failed");

    MUST_OR_RET(pwrite(fd, buf, read, write_pos) == read, -1, "pwrite failed");

    to_move -= read;
    read_pos -= read;
    write_pos -= read;
    MUST(to_move >= 0, "to_move < 0");
  }

  // fill the gap with `fill`
  memset(buf, fill, sizeof(buf));
  write_pos = off;
  size_t to_fill = len;
  while (to_fill > 0) {
    to_read = to_fill > sizeof(buf) ? sizeof(buf) : to_fill;
    size_t written = pwrite(fd, buf, to_read, write_pos);
    MUST_OR_RET(written > 0, -1, "pwrite failed");

    to_fill -= written;
    write_pos += written;
  }

  // restore the file pointer
  where = where <= off ? where : off + len;
  lseek(fd, where, SEEK_SET);
#else
  UNIMPLEMENTED();
#endif
  return 0;
}
