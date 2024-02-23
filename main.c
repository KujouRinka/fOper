#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "global.h"

#include "std_file_wrapper.h"
#include "action.h"
#include "common.h"

void clean() {
  for (int i = 0; i < external_files_count; i++) {
    fclose(external_files[i].file);
  }
  free(external_files);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    FATAL("Usage: %s <config file>\n", argv[0]);
  }
  struct Config *config = parse_config(argv[1]);

  printf("gen_filename: %s\n", config->gen_filename);
  printf("extern_file_size: %d\n", config->extern_file_size);
  for (int i = 0; i < config->extern_file_size; i++) {
    printf("external_file[%d]: %s\n", i, config->extern_files[i]);
  }
  printf("create_method: %s\n", config->create_method);
  printf("operation_count: %d\n", config->operation_count);
  for (int i = 0; i < config->operation_count; i++) {
    printf("operation[%d]:\n", i);
    printf("  kv_cnt: %d\n", config->operations[i].kv_cnt);
    for (int j = 0; j < config->operations[i].kv_cnt; j++) {
      printf("  kvs[%d]: %s=%s\n", j, config->operations[i].kvs[j].key, config->operations[i].kvs[j].value);
    }
  }

  // open external files
  external_files_count = config->extern_file_size;
  external_files = malloc(sizeof(string2file) * config->extern_file_size);
  for (int i = 0; i < config->extern_file_size; ++i) {
    external_files[i].filename = config->extern_files[i];
    external_files[i].file = fopen(config->extern_files[i], "r");
    if (external_files[i].file == NULL) {
      FATAL("Could not open file %s\n", config->extern_files[i]);
    }
  }

  // TODO: create method
  void *output = NULL;
  if (strcmp(config->create_method, "default") == 0) {
    output = std_file_init(config->gen_filename, "w+");
    if (output == NULL) {
      FATAL("Could not open file %s\n", config->gen_filename);
    }
  } else {
    FATAL("Unknown create method: %s\n", config->create_method);
  }

  for (int i = 0; i < config->operation_count; ++i) {
    char *action = OperationGet(&config->operations[i], "action");
    if (strcmp(action, "syscall") == 0) {
      printf("syscall %s\n", OperationGet(&config->operations[i], "call"));
      process_syscall(output, &config->operations[i]);
    } else if (strcmp(action, "write") == 0) {
      char *verb = OperationGet(&config->operations[i], "verb");
      if (strcmp(verb, "file") == 0) {
        char *filename = OperationGet(&config->operations[i], "filename");
        char *offset_str = OperationGet(&config->operations[i], "offset");
        char *len_str = OperationGet(&config->operations[i], "len");
        long offset, len;
        MUST_STR2L(offset, offset_str);
        MUST_STR2L(len, len_str);
        int ret = write_file(output, filename, offset, len);
        if (ret != 0) {
          FATAL("dwrite_file failed\n");
        }
      } else if (strcmp(verb, "random") == 0) {
        char *len_str = OperationGet(&config->operations[i], "len");
        long len;
        MUST_STR2L(len, len_str);
        int ret = write_rand(output, len);
        if (ret != 0) {
          FATAL("write_rand failed\n");
        }
      } else {
        FATAL("Unknown verb: %s\n", OperationGet(&config->operations[i], "verb"));
      }
    } else if (strcmp(action, "delete") == 0) {
      printf("seek %s\n", OperationGet(&config->operations[i], "verb"));
    } else {
      FATAL("Unknown action: %s\n", OperationGet(&config->operations[i], "action"));
    }
  }
  clean();
  free_config(config);
  return 0;
}
