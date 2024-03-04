#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "global.h"

#include "cmd/cmd.h"
#include "config/config.h"

void clean() {
  for (int i = 0; i < external_files_count; i++) {
    fclose(external_files[i].file);
  }
  free(external_files);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    FATAL("Usage: %s <config file>", argv[0]);
  }

  struct ConfigObj *config = parse_config_v2(argv[1]);
  MUST(config != NULL, "config file is null");
  CONFIG_REQUIRE(config, CONFIG_TYPE_MAP);

  struct ConfigObj *task_type_obj = CONFIG_TRAIT(config, as_map, "task-type");
  MUST(task_type_obj != NULL, "task-type is null");
  CONFIG_REQUIRE(task_type_obj, CONFIG_TYPE_BYTE);
  char *task_type = CONFIG_TRAIT(task_type_obj, as_string);

  int ret;
  if (strcmp(task_type, "gen-file") == 0) {
    ret = cmd_gen_file(config);
  } else if (strcmp(task_type, "copy-file") == 0) {
    ret = cmd_copy_file(config);
  } else {
    ret = -1;
  }

  if (ret != 0) {
    EPRINTF("Task failed");
  }

  clean();
  CONFIG_TRAIT(config, free);
  return ret;
}
