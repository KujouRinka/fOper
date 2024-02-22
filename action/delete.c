#include "delete.h"

#include <stdio.h>

#include "file_interface.h"

int delete_offset(void *read_write_seeker, size_t offset) {
  struct FileTrait *rw = (struct FileTrait *) read_write_seeker;
  return rw->truncate(rw, offset);
}
