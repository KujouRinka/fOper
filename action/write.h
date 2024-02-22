#ifndef WRITE_H
#define WRITE_H

#include <stdio.h>
#include <stddef.h>

int write_file(void *read_write_seeker, const char *filename, ssize_t offset, size_t len);
int write_rand(void *read_write_seeker, size_t len);

#endif
