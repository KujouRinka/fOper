#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__linux__)
#include "arch/linux/common.h"
#else

#endif

#include "global.h"

#define FN_META() \
  size_t __free_list_start_len_to_free_in_this_function__ = free_list.len

#define FN_CLEAN() \
  free_list_free(__free_list_start_len_to_free_in_this_function__)

#define FN_VOID_TO_INT_ADAPTER_DEC(fn_name) \
  int fn_name(void *arg);

#define FN_VOID_TO_INT_ADAPTER_DEF(fn_name, fn, arg_type) \
  int fn_name(void *arg) {                            \
    fn((arg_type) arg);                               \
    return 0;                                         \
  }

#define UNIMPLEMENTED()                                            \
  do {                                                             \
    fprintf(stderr, "Unimplemented: %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                       \
  } while (0)

#define UNREACHABLE()                                            \
  do {                                                           \
    fprintf(stderr, "Unreachable: %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                     \
  } while (0)

#define FATAL(fmt, ...)                                                              \
  do {                                                                               \
    fprintf(stderr, "Fatal: " fmt " at %s:%d\n", ##__VA_ARGS__, __FILE__, __LINE__); \
    exit(1);                                                                         \
  } while (0)

#define EPRINTF(fmt, ...)                                                  \
  do {                                                                     \
    fprintf(stderr, fmt " at %s:%d\n", ##__VA_ARGS__, __FILE__, __LINE__); \
  } while (0)

#define LOG(fmt, ...)                         \
  do {                                        \
    fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
  } while (0)

#define EXPECT_EQ(a, b, msg) \
  do {                       \
    if ((a) != (b)) {        \
      FATAL(msg);            \
    }                        \
  } while (0)

#define EXPECT_NE(a, b, msg) \
  do {                       \
    if ((a) == (b)) {        \
      FATAL(msg);            \
    }                        \
  } while (0)

#define MUST(expr, fmt, args...) \
  do {                           \
    if (!(expr)) {               \
      FATAL(fmt, ##args);        \
    }                            \
  } while (0)

#define MUST_OR_RET(expr, ret, fmt, args...) \
  do {                                       \
    if (!(expr)) {                           \
      EPRINTF(fmt, ##args);                  \
      return (ret);                          \
    }                                        \
  } while (0)

#define MUST_OR_FREE_RET(expr, ret, fmt, args...)                       \
  do {                                                                  \
    if (!(expr)) {                                                      \
      EPRINTF(fmt, ##args);                                             \
      free_list_free(__free_list_start_len_to_free_in_this_function__); \
      return (ret);                                                     \
    }                                                                   \
  } while (0)

#define MUST_STR2L(var, str)            \
  do {                                  \
    char *endptr;                       \
    var = strtol(str, &endptr, 10);     \
    if (*endptr != '\0') {              \
      FATAL("Invalid number: %s", str); \
    }                                   \
  } while (0)

#define DUP_OBJ(dst_ptr, src_ptr)               \
  do {                                          \
    dst_ptr = malloc(sizeof(*src_ptr));         \
    if (dst_ptr == NULL) {                      \
      FATAL("malloc failed");                   \
    }                                           \
    memcpy(dst_ptr, src_ptr, sizeof(*src_ptr)); \
  } while (0)

#define MUST_STR2LL(var, str)           \
  do {                                  \
    char *endptr;                       \
    var = strtoll(str, &endptr, 10);    \
    if (*endptr != '\0') {              \
      FATAL("Invalid number: %s", str); \
    }                                   \
  } while (0)

#define MUST_STR2INT(var, str)          \
  do {                                  \
    char *endptr;                       \
    var = strtol(str, &endptr, 10);     \
    if (*endptr != '\0') {              \
      FATAL("Invalid number: %s", str); \
    }                                   \
  } while (0)

#define MUST_STR2UL(var, str)           \
  do {                                  \
    char *endptr;                       \
    var = strtoul(str, &endptr, 10);    \
    if (*endptr != '\0') {              \
      FATAL("Invalid number: %s", str); \
    }                                   \
  } while (0)

#define MUST_STR2ULL(var, str)          \
  do {                                  \
    char *endptr;                       \
    var = strtoull(str, &endptr, 10);   \
    if (*endptr != '\0') {              \
      FATAL("Invalid number: %s", str); \
    }                                   \
  } while (0)

#define TRAIT(obj, trait_type, method, args...) \
  ((*(struct trait_type **) (obj))->method(obj, ##args))

int append_to_arr(void **arr, size_t *len, size_t *cap, void *data, size_t data_size);
int high_water_alloc(void **buf, size_t *bufsize, size_t newsize);

const char *filepath_base_begin(const char *filepath);
const char *filepath_dir_end(const char *filepath);

FN_VOID_TO_INT_ADAPTER_DEC(free_adapter);

#endif
