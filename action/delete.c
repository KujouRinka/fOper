#include "delete.h"

#include <stdio.h>

#include "file_trait.h"

int delete_offset(void *file_trait, size_t offset) {
  struct FileTrait *ft = *(struct FileTrait **) file_trait;
  return ft->truncate(file_trait, offset);
}
