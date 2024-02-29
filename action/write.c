#include "write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "file_trait.h"
#include "global.h"

int write_file(void *file_trait, const char *filename, ssize_t offset, size_t len) {
  FILE *src = NULL;
  for (int i = 0; i < external_files_count; ++i) {
    if (strcmp(external_files[i].filename, filename) == 0) {
      src = external_files[i].file;
      break;
    }
  }
  MUST_OR_RET(src != NULL, -1, "file not found");

  struct FileTrait *ft = *(struct FileTrait **) file_trait;
  // copy len bytes from src to rw
  if (fseek(src, offset, SEEK_SET) != 0) {
    return -1;
  }
  char buf[1024];
  while (len > 0) {
    size_t to_read = len > sizeof(buf) ? sizeof(buf) : len;
    size_t read = fread(buf, 1, to_read, src);
    MUST_OR_RET(read > 0, -1, "fread failed");

    // write n bytes to rw
    MUST_OR_RET(ft->write(file_trait, buf, read) == 0, -1, "write failed");

    len -= read;
  }
  return 0;
}

int write_rand(void *file_trait, size_t len) {
  struct FileTrait *ft = *(struct FileTrait **) file_trait;
  // write len bytes to rw
  char buf[1024];
  while (len > 0) {
    size_t to_write = len > sizeof(buf) ? sizeof(buf) : len;
    for (size_t i = 0; i < to_write; i++) {
      buf[i] = rand() % 256;
    }
    MUST_OR_RET(ft->write(file_trait, buf, to_write) == 0, -1, "write failed");

    len -= to_write;
  }
  return 0;
}
