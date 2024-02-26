#include "common.h"

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
