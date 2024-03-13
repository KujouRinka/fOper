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

int filepath_cmp(const char *fp1, const char *fp2) {
  int root_a = fp1[0] == '/';
  int root_b = fp2[0] == '/';
  if (root_a != root_b) {
    return 1;
  }

  char *str1, *str2;
  if (root_a) {
    str1 = filepath_simplify(fp1 + 1);
    str2 = filepath_simplify(fp2 + 1);
  } else {
    str1 = filepath_simplify(fp1);
    str2 = filepath_simplify(fp2);
  }
  fp1 = str1;
  fp2 = str2;
  while (*fp1 != '\0' && *fp2 != '\0') {
    // skip slash
    while (*fp1 == '/') {
      fp1++;
    }
    while (*fp2 == '/') {
      fp2++;
    }
    if (*fp1 == '\0' || *fp2 == '\0') {
      break;
    }
    if (*fp1++ != *fp2++) {
      free(str1);
      free(str2);
      return 1;
    }
  }
  int ret = *fp1 != *fp2;
  free(str1);
  free(str2);
  return ret;
}

static char **split(const char *s, char delim, int *returnSize) {
  int n = strlen(s);
  char **ret = (char **) malloc(sizeof(char *) * n);
  int pos = 0;
  int curr = 0;
  int len = 0;

  while (pos < n) {
    while (pos < n && s[pos] == delim) {
      ++pos;
    }
    curr = pos;
    while (pos < n && s[pos] != delim) {
      ++pos;
    }
    if (curr < n) {
      ret[len] = (char *) malloc(sizeof(char) * (pos - curr + 1));
      strncpy(ret[len], s + curr, pos - curr);
      ret[len][pos - curr] = '\0';
      ++len;
    }
  }
  *returnSize = len;
  return ret;
}

static char *filepath_simplify(const char *path) {
  int names_size = 0;
  int n = strlen(path);
  char **names = split(path, '/', &names_size);
  char **stack = (char **) malloc(sizeof(char *) * names_size);
  int stackSize = 0;
  for (int i = 0; i < names_size; ++i) {
    if (!strcmp(names[i], "..")) {
      if (stackSize > 0) {
        --stackSize;
      }
    } else if (strcmp(names[i], ".")) {
      stack[stackSize] = names[i];
      ++stackSize;
    }
  }

  char *ret = (char *) malloc(sizeof(char) * (n + 1));
  int curr = 0;
  if (stackSize == 0) {
    ret[curr] = '/';
    ++curr;
  } else {
    for (int i = 0; i < stackSize; ++i) {
      ret[curr] = '/';
      ++curr;
      strcpy(ret + curr, stack[i]);
      curr += strlen(stack[i]);
    }
  }
  ret[curr] = '\0';
  for (int i = 0; i < names_size; ++i) {
    free(names[i]);
  }
  free(names);
  free(stack);
  return ret;
}

FN_VOID_TO_INT_ADAPTER_DEF(free_adapter, free, void *);
