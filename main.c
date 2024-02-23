#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "config_inner.h"
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

int main() {
  struct ConfigObj *config = parse_config_v2("./config/config.yaml");
  MUST(config != NULL, "config file is null");
  CONFIG_REQUIRE(config, CONFIG_TYPE_MAP);

  struct ConfigObj *gen_filename_obj = CONFIG_TRAIT(config, get_map, "gen-file");
  MUST(gen_filename_obj != NULL, "gen-file is null");
  CONFIG_REQUIRE(gen_filename_obj, CONFIG_TYPE_STRING);

  char *gen_filename = CONFIG_TRAIT(gen_filename_obj, get_string);
  MUST(strcmp(gen_filename, "") != 0, "gen-file is empty");

  printf("gen-file: %s\n", gen_filename);

  struct ConfigObj *external_files_obj = CONFIG_TRAIT(config, get_map, "external-files");
  CONFIG_REQUIRE(external_files_obj, CONFIG_TYPE_ARRAY);
  printf("extern-files: \n");
  int extern_files_count = CONFIG_TRAIT(external_files_obj, get_array_size);
  for (int i = 0; i < extern_files_count; ++i) {
    struct ConfigObj *external_file_obj = CONFIG_TRAIT(external_files_obj, get_array, i);
    MUST(external_file_obj != NULL, "external file is null");
    CONFIG_REQUIRE(external_file_obj, CONFIG_TYPE_STRING);
    printf("  %s\n", CONFIG_TRAIT(external_file_obj, get_string));
  }

  struct ConfigObj *operations_obj = CONFIG_TRAIT(config, get_map, "operation");
  MUST(operations_obj != NULL, "operation is null");
  CONFIG_REQUIRE(operations_obj, CONFIG_TYPE_ARRAY);
  printf("operation: \n");
  int operations_count = CONFIG_TRAIT(operations_obj, get_array_size);
  for (int i = 0; i < operations_count; ++i) {
    struct ConfigObj *operation_obj = CONFIG_TRAIT(operations_obj, get_array, i);
    MUST(operation_obj != NULL, "operation is null");
    CONFIG_REQUIRE(operation_obj, CONFIG_TYPE_MAP);
    printf("  operation [%d]: \n", i);
    int kv_count = CONFIG_TRAIT(operation_obj, get_map_size);
    for (int j = 0; j < kv_count; ++j) {
      struct objKv *kv = CONFIG_TRAIT(operation_obj, get_map_by_index, j);
      MUST(kv != NULL, "kv is null");
      CONFIG_REQUIRE(kv->key, CONFIG_TYPE_STRING);
      printf("    %s: ", kv->key);
      if (CONFIG_TRAIT(kv->value, type) == CONFIG_TYPE_STRING) {
        printf("%s\n", CONFIG_TRAIT(kv->value, get_string));
      } else if (CONFIG_TRAIT(kv->value, type) == CONFIG_TYPE_INT) {
        printf("%d\n", CONFIG_TRAIT(kv->value, get_int));
      } else if (CONFIG_TRAIT(kv->value, type) == CONFIG_TYPE_FLOAT) {
        printf("%f\n", CONFIG_TRAIT(kv->value, get_float));
      } else if (CONFIG_TRAIT(kv->value, type) == CONFIG_TYPE_BOOL) {
        printf("%d\n", CONFIG_TRAIT(kv->value, get_bool));
      } else {
        FATAL("Invalid type: %d", CONFIG_TRAIT(kv->value, type));
      }
    }
  }

  return 0;
}
