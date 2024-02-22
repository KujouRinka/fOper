#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

typedef struct {
  char *filename;
  FILE *file;
} string2file;

extern int external_files_count;
extern string2file *external_files;

#endif