#include "copy_file.h"

#include "action/copy_file/copy_file.h"
#include "common.h"
#include "config/config.h"

int cmd_copy_file(struct ConfigObj *config) {
  char *target_str = CONFIG_TRAIT(CONFIG_TRAIT(config, as_map, "target"), as_string);
  char *source_str = CONFIG_TRAIT(CONFIG_TRAIT(config, as_map, "source"), as_string);

  // get file type of target and source
  int a, b, c, d;
  a = is_reg_file(target_str);
  b = is_dir(target_str);
  c = is_reg_file(source_str);
  d = is_dir(source_str);
  MUST_OR_RET(!(a == 0 && b == 0), -1, "target is not a file or directory");
  MUST_OR_RET(!(c == 0 && d == 0), -1, "source is not a file or directory");

  if (a != 0 && c != 0) {
    // dst and src are file
    MUST_OR_RET(copy_file(target_str, source_str, config) == 0, -1, "copy file failed");
  } else if (b != 0 && c != 0) {
    // dst is directory, src is file
    MUST_OR_RET(copy_file_to_dir(target_str, source_str, config) == 0, -1, "copy file to dir failed");
  } else if (b != 0 && d != 0) {
    // dst and src are directory
    MUST_OR_RET(copy_dir(target_str, source_str, config) == 0, -1, "copy dir failed");
  } else {
    EPRINTF("Invalid file type\n");
    return -1;
  }

  return 0;
}
