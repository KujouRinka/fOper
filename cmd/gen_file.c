#include "gen_file.h"

#include "action.h"
#include "global.h"

#include "common.h"
#include "config.h"
#include "std_file_wrapper.h"

int cmd_gen_file(struct ConfigObj *config) {
  char *gen_file = CONFIG_TRAIT(CONFIG_TRAIT(config, as_map, "target"), as_string);
  MUST(gen_file != NULL, "gen-file is null");
  printf("gen-file: %s\n", gen_file);

  // open external files
  struct ConfigObj *extern_files = CONFIG_TRAIT(config, as_map, "external-file");
  MUST(extern_files != NULL, "external-file is null");
  CONFIG_REQUIRE(extern_files, CONFIG_TYPE_ARRAY);
  size_t extern_files_size = CONFIG_TRAIT(extern_files, as_array_size);
  external_files_count = extern_files_size;
  external_files = malloc(sizeof(string2file) * extern_files_size);
  for (size_t i = 0; i < extern_files_size; i++) {
    external_files[i].filename = CONFIG_TRAIT(CONFIG_TRAIT(extern_files, as_array, i), as_string);
    external_files[i].file = fopen(external_files[i].filename, "r");
  }

  // TODO: create method
  void *output = NULL;
  struct ConfigObj *cm = CONFIG_TRAIT(config, as_map, "create-method");
  MUST(cm != NULL, "create-method is null");
  CONFIG_REQUIRE(cm, CONFIG_TYPE_BYTE);
  char *create_method = CONFIG_TRAIT(cm, as_string);
  MUST(create_method != NULL, "create-method is null");
  if (strcmp(create_method, "default") == 0) {
    output = std_file_init(gen_file, "w+");
    if (output == NULL) {
      FATAL("Could not open file %s\n", gen_file);
    }
  } else {
    FATAL("Unknown create method: %s\n", create_method);
  }

  // do oper
  struct ConfigObj *operations = CONFIG_TRAIT(config, as_map, "operation");
  MUST(operations != NULL, "operation is null");
  CONFIG_REQUIRE(operations, CONFIG_TYPE_ARRAY);
  size_t operation_count = CONFIG_TRAIT(operations, as_array_size);

  int ret;
  for (int i = 0; i < operation_count; ++i) {
    struct ConfigObj *operation = CONFIG_TRAIT(operations, as_array, i);
    CONFIG_REQUIRE(operation, CONFIG_TYPE_MAP);
    char *action = CONFIG_TRAIT(CONFIG_TRAIT(operation, as_map, "action"), as_string);
    MUST(action != NULL, "action is null");
    if (strcmp(action, "syscall") == 0) {
      char *call = CONFIG_TRAIT(CONFIG_TRAIT(operation, as_map, "call"), as_string);
      MUST(call != NULL, "call is null");
      printf("syscall %s\n", call);
      ret = process_syscall(output, (struct ConfigMap *) operation);
      if (ret != 0) {
        EPRINTF("process_syscall failed\n");
        return ret;
      }
    } else if (strcmp(action, "write") == 0) {
      char *verb = CONFIG_TRAIT(CONFIG_TRAIT(operation, as_map, "verb"), as_string);
      MUST(verb != NULL, "verb is null");
      if (strcmp(verb, "file") == 0) {
        char *filename = CONFIG_TRAIT(CONFIG_TRAIT(operation, as_map, "filename"), as_string);
        char *offset_str = CONFIG_TRAIT(CONFIG_TRAIT(operation, as_map, "offset"), as_string);
        char *len_str = CONFIG_TRAIT(CONFIG_TRAIT(operation, as_map, "len"), as_string);
        MUST(filename != NULL, "filename is null");
        MUST(offset_str != NULL, "offset is null");
        MUST(len_str != NULL, "len is null");

        long offset, len;
        MUST_STR2L(offset, offset_str);
        MUST_STR2L(len, len_str);
        ret = write_file(output, filename, offset, len);
        if (ret != 0) {
          EPRINTF("dwrite_file failed\n");
          return ret;
        }
      } else if (strcmp(verb, "random") == 0) {
        char *len_str = CONFIG_TRAIT(CONFIG_TRAIT(operation, as_map, "len"), as_string);
        MUST(len_str != NULL, "len is null");
        long len;
        MUST_STR2L(len, len_str);
        ret = write_rand(output, len);
        if (ret != 0) {
          EPRINTF("write_rand failed\n");
          return ret;
        }
      } else {
        FATAL("Unknown verb: %s\n", verb);
      }
    } else if (strcmp(action, "move") == 0) {
      ret = action_move(output, operation);
      if (ret != 0) {
        EPRINTF("action_move failed\n");
        return ret;
      }
    } else {
      FATAL("Unknown action: %s\n", action);
    }
  }

  TRAIT(output, FileTrait, close);
  return ret;
}
