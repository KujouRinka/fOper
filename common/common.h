#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>

#define UNIMPLEMENTED() do { \
  fprintf(stderr, "Unimplemented: %s:%d\n", __FILE__, __LINE__); \
  exit(1); \
} while (0)

#define FATAL(fmt, ...) do { \
  fprintf(stderr, "Fatal: " fmt " at %s:%d\n", ##__VA_ARGS__, __FILE__, __LINE__); \
  exit(1); \
} while (0)

#define EPRINTF(fmt, ...) do { \
  fprintf(stderr, fmt, ##__VA_ARGS__); \
} while (0)

#endif
