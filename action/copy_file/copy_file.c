#include "copy_file.h"

#include <errno.h>

#include "common.h"
#include "config/config.h"
#include "file/std_file_wrapper.h"
#include "property.h"

#if defined(__linux__)
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file/arch/linux/dir.h"
#include "file/arch/linux/foper.h"
#endif

int copy_file(const char *dst, const char *src, struct ConfigObj *config) {
  FN_META();

  // copy file src to dst
  // TODO: How to open file?
  struct FileTrait **src_file = std_file_init(src, "r");
  MUST_OR_FREE_RET(src_file != NULL, -1, "open file failed: %s: %s", src, strerror(errno));
  free_list_add(src_file, (*src_file)->close);
  struct FileTrait **dst_file = std_file_init(dst, "w+");
  MUST_OR_FREE_RET(dst_file != NULL, -1, "open file failed: %s: %s", dst, strerror(errno));
  free_list_add(dst_file, (*dst_file)->close);

  MUST_OR_FREE_RET(copy(dst_file, src_file) == 0, -1, "copy file failed");

  MUST_OR_FREE_RET(write_property(dst_file, src_file, config) == 0, -1, "write property failed");

  FN_CLEAN();
  return 0;
}

int copy_file_to_dir(const char *dst, const char *src, struct ConfigObj *config) {
  // get src filename
  const char *filename = filepath_base_begin(src);

  // get dst filename
  char *dst_filename = malloc(strlen(dst) + strlen(filename) + 2);
  MUST_OR_RET(dst_filename != NULL, -1, "malloc failed");
  sprintf(dst_filename, "%s/%s", dst, filename);

  int ret = copy_file(dst_filename, src, config);
  free(dst_filename);
  return ret;
}

static int vdcb(struct dirent *entry, const char *ori_path, const char *step_path, void *arg) {
  FN_META();

  const char *dst_dir = (const char *) ((uint64_t *) arg)[0];
  struct ConfigObj *config = (struct ConfigObj *) ((uint64_t *) arg)[1];
#if defined(__linux__)
  // make up dst filename
  char *dst_filename = malloc(strlen(dst_dir) + strlen(step_path) + strlen(entry->d_name) + 3);
  MUST_OR_FREE_RET(dst_filename != NULL, -1, "malloc failed");
  free_list_add(dst_filename, free_adapter);
  sprintf(dst_filename, "%s/%s%s%s", dst_dir, step_path, step_path[0] == '\0' ? "" : "/", entry->d_name);
  // make up src filename
  char *src_filename = malloc(strlen(ori_path) + strlen(step_path) + strlen(entry->d_name) + 3);
  MUST_OR_FREE_RET(src_filename != NULL, (free(dst_filename), -1), "malloc failed");
  free_list_add(src_filename, free_adapter);
  sprintf(src_filename, "%s/%s%s%s", ori_path, step_path, step_path[0] == '\0' ? "" : "/", entry->d_name);

  // skip exclude
  struct ConfigObj *exclude = CONFIG_TRAIT(config, as_map, "exclude");
  size_t exclude_len = CONFIG_TRAIT(exclude, as_array_size);
  char *ptr_to_cmp = src_filename + strlen(ori_path) + 1;
  for (size_t i = 0; i < exclude_len; i++) {
    struct ConfigObj *exclude_filename = CONFIG_TRAIT(exclude, as_array, i);
    char *exclude_str = CONFIG_TRAIT(exclude_filename, as_string);
    if (filepath_cmp(ptr_to_cmp, exclude_str) == 0) {
      FN_CLEAN();
      return 1;
    }
  }

  // create dir if not exist
  if (entry->d_type == DT_DIR) {
    if (access(dst_filename, F_OK) != 0) {
      MUST_OR_FREE_RET(mkdir(dst_filename, 0755) == 0, (free(dst_filename), -1), "mkdir failed: %s", strerror(errno));
    }

    fnwrite_dir_property(dst_filename, src_filename, config);

    FN_CLEAN();
    return 0;
  }
  // skip if is not file
  if (entry->d_type != DT_REG) {
    FN_CLEAN();
    return 0;
  }

  // copy file
  int ret = copy_file(dst_filename, src_filename, config);
  MUST_OR_FREE_RET(ret == 0, -1, "copy_file failed");

  FN_CLEAN();
#else
  UNIMPLEMENTED();
#endif
  return 0;
}

int copy_dir(const char *dst, const char *src, struct ConfigObj *config) {
  struct {
    uint64_t dir_dst;
    uint64_t config_ptr;
  } arg;
  arg.dir_dst = (uint64_t) dst;
  arg.config_ptr = (uint64_t) config;

  // copy src to dst
#if defined(__linux__)
  if (access(dst, F_OK) != 0) {
    MUST_OR_RET(mkdir(dst, 0755) == 0, -1, "mkdir failed: %s", strerror(errno));
    MUST_OR_RET(fnwrite_dir_property(dst, src, config) == 0, -1, "write property failed");
  }
  MUST_OR_RET(visit_dir(src, vdcb, (void *) &arg) == 0, -1, "visit_dir failed: dst: %s, src: %s", dst, src);
#else
  UNIMPLEMENTED();
#endif

  return 0;
}
