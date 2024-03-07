#include "global.h"

#include "common.h"

size_t external_files_count = 0;
string2file *external_files = NULL;

FreeList free_list = {
    .pfs = NULL,
    .len = 0,
    .cap = 0,
};

void free_list_init() {
  free_list.pfs = NULL;
  free_list.len = 0;
  free_list.cap = 0;
}

void free_list_add(void *ptr, int (*free_func)(void *)) {
  struct pf pf = {
      .ptr = ptr,
      .free_func = free_func,
  };
  MUST(append_to_arr((void **) &free_list.pfs, &free_list.len, &free_list.cap, &pf, sizeof(pf)) == 0, "append_to_arr failed");
}

void free_list_free(size_t from) {
  for (size_t i = from; i < free_list.len; i++) {
    MUST(free_list.pfs[i].free_func(free_list.pfs[i].ptr) == 0, "free_func failed");
  }
  free_list.len = from;
}

void free_list_free_all() {
  free_list_free(0);
  free(free_list.pfs);
}
