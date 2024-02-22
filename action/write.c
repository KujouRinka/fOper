#include "write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_interface.h"
#include "global.h"
#include "common.h"

int write_file(void *read_write_seeker, const char *filename, ssize_t offset, size_t len) {
  FILE *src = NULL;
  for (int i = 0; i < external_files_count; ++i) {
    if (strcmp(external_files[i].filename, filename) == 0) {
      src = external_files[i].file;
      break;
    }
  }
  if (src == NULL) {
    EPRINTF("Could not find file %s\n", filename);
    return -1;
  }

  struct FileTrait *rw = (struct FileTrait *) read_write_seeker;
  // copy len bytes from src to rw
  if (fseek(src, offset, SEEK_SET) != 0) {
    return -1;
  }
  char buf[1024];
  while (len > 0) {
    size_t to_read = len > sizeof(buf) ? sizeof(buf) : len;
    size_t read = fread(buf, 1, to_read, src);
    if (read == 0) {
      return -1;
    }
    // write n bytes to rw
    if (rw->write(rw, buf, read) != 0) {
      return -1;
    }
    len -= read;
  }
  return 0;
}

int write_rand(void *read_write_seeker, size_t len) {
  struct FileTrait *rw = (struct FileTrait *) read_write_seeker;
  // write len bytes to rw
  char buf[1024];
  while (len > 0) {
    size_t to_write = len > sizeof(buf) ? sizeof(buf) : len;
    for (size_t i = 0; i < to_write; i++) {
      buf[i] = rand() % 256;
    }
    if (rw->write(rw, buf, to_write) != 0) {
      return -1;
    }
    len -= to_write;
  }
  return 0;
}
