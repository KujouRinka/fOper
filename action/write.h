#ifndef WRITE_H
#define WRITE_H

#include <stddef.h>
#include <stdio.h>

int write_file(void *file_trait, const char *filename, ssize_t offset, size_t len);
int write_rand(void *file_trait, size_t len);

#endif
