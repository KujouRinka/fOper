#include "common.h"

#include <errno.h>
#include <string.h>

int append_to_arr(void **arr, size_t *len, size_t *cap, void *data, size_t data_size) {
  if (*arr == NULL) {
    *arr = malloc(data_size);
    if (*arr == NULL) {
      return -1;
    }
    *len = 0;
    *cap = 1;
  } else if (*len == *cap) {
    *cap *= 2;
    *arr = realloc(*arr, *cap * data_size);
    if (*arr == NULL) {
      return -1;
    }
    memset((char *) *arr + *len * data_size, 0, *cap - *len);
  }
  memcpy((char *) *arr + *len * data_size, data, data_size);
  *len += 1;
  return 0;
}

int high_water_alloc(void **buf, size_t *bufsize, size_t newsize) {
#define CHUNK_SIZE 256
  /*
	 * Goal here is to avoid unnecessary memory allocations by
	 * using static buffers which only grow when necessary.
	 * Size is increased in fixed size chunks (CHUNK_SIZE).
	 */
  if (*bufsize < newsize) {
    void *newbuf;

    newsize = (newsize + CHUNK_SIZE - 1) & ~(CHUNK_SIZE - 1);
    MUST_OR_RET((newbuf = realloc(*buf, newsize)) != NULL, -1, "realloc failed: %s", strerror(errno));

    *buf = newbuf;
    *bufsize = newsize;
  }
  return 0;
}

const char *filepath_base_begin(const char *filepath) {
  const char *base = strrchr(filepath, '/');
  if (base == NULL) {
    base = filepath;
  } else {
    base++;
  }
  return base;
}

const char *filepath_dir_end(const char *filepath) {
  const char *end = strrchr(filepath, '/');
  if (end == NULL) {
    end = filepath;
  }
  return end;
}

FN_VOID_TO_INT_ADAPTER_DEF(free_adapter, free, void *);
