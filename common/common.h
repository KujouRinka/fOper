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

#define MUST_STR2L(var, str) do { \
  char *endptr; \
  var = strtol(str, &endptr, 10); \
  if (*endptr != '\0') { \
    FATAL("Invalid number: %s", str); \
  } \
} while (0)

#define MUST_STR2LL(var, str) do { \
  char *endptr; \
  var = strtoll(str, &endptr, 10); \
  if (*endptr != '\0') { \
    FATAL("Invalid number: %s", str); \
  } \
} while (0)

#define MUST_STR2INT(var, str) do { \
  char *endptr; \
  var = strtol(str, &endptr, 10); \
  if (*endptr != '\0') { \
    FATAL("Invalid number: %s", str); \
  } \
} while (0)

#define MUST_STR2UL(var, str) do { \
  char *endptr; \
  var = strtoul(str, &endptr, 10); \
  if (*endptr != '\0') { \
    FATAL("Invalid number: %s", str); \
  } \
} while (0)

#define MUST_STR2ULL(var, str) do { \
  char *endptr; \
  var = strtoull(str, &endptr, 10); \
  if (*endptr != '\0') { \
    FATAL("Invalid number: %s", str); \
  } \
} while (0)

#endif
