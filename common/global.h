#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

typedef struct {
  char *filename;
  FILE *file;
} string2file;

extern size_t external_files_count;
extern string2file *external_files;

struct pf {
  void *ptr;
  int (*free_func)(void *);
};

typedef struct {
  struct pf *pfs;
  size_t len;
  size_t cap;
} FreeList;

void free_list_init();
void free_list_add(void *ptr, int (*free_func)(void *));
void free_list_free(size_t from);
void free_list_free_all();

extern FreeList free_list;

#endif
