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

  // copy file src to dst
  // TODO: How to open file?
  struct FileTrait *src_file = std_file_init(src, "r");
  MUST_OR_RET(src_file != NULL, -1, "open file failed: %s: %s", src, strerror(errno));
  struct FileTrait *dst_file = std_file_init(dst, "w+");
  MUST_OR_RET(dst_file != NULL, (TRAIT(src_file, FileTrait, close), -1), "open file failed: %s: %s", dst, strerror(errno));

  MUST_OR_RET(copy(dst_file, src_file) == 0, -1, "copy file failed");

  MUST_OR_RET(write_property(dst_file, src_file, config) == 0, -1, "write property failed");

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

#if defined(__linux__)
static int vdcb(struct dirent *entry, const char *ori_path, const char *step_path, void *arg) {
  const char *dst_dir = (const char *) ((uint64_t *) arg)[0];
  struct ConfigObj *config = (struct ConfigObj *) ((uint64_t *) arg)[1];

  // create dir if not exist
  if (entry->d_type == DT_DIR) {
    char *dst_dirname = malloc(strlen(dst_dir) + strlen(step_path) + strlen(entry->d_name) + 3);
    MUST_OR_RET(dst_dirname != NULL, -1, "malloc failed");
    sprintf(dst_dirname, "%s/%s/%s", dst_dir, step_path, entry->d_name);
    if (access(dst_dirname, F_OK) != 0) {
      MUST_OR_RET(mkdir(dst_dirname, 0755) == 0, (free(dst_dirname), -1), "mkdir failed: %s", strerror(errno));
    }
    free(dst_dirname);

    char *src_filename = malloc(strlen(ori_path) + strlen(step_path) + strlen(entry->d_name) + 3);
    MUST_OR_RET(src_filename != NULL, -1, "malloc failed");
    sprintf(src_filename, "%s/%s/%s", ori_path, step_path, entry->d_name);

    fnwrite_dir_property(dst_dirname, src_filename, config);
    return 0;
  }
  // skip if is not file
  if (entry->d_type != DT_REG) {
    return 0;
  }

  // make up src filename
  char *src_filename = malloc(strlen(ori_path) + strlen(step_path) + strlen(entry->d_name) + 3);
  MUST_OR_RET(src_filename != NULL, -1, "malloc failed");
  sprintf(src_filename, "%s/%s/%s", ori_path, step_path, entry->d_name);

  // make up dst filename
  char *dst_filename = malloc(strlen(dst_dir) + strlen(step_path) + strlen(entry->d_name) + 3);
  MUST_OR_RET(dst_filename != NULL, -1, "malloc failed");
  sprintf(dst_filename, "%s/%s/%s", dst_dir, step_path, entry->d_name);

  // copy file
  int ret = copy_file(dst_filename, src_filename, config);
  MUST_OR_RET(ret == 0, (free(src_filename), free(dst_filename), -1), "copy_file failed");
  free(src_filename);
  free(dst_filename);

  return 0;
}
#endif

int copy_dir(const char *dst, const char *src, struct ConfigObj *config) {
  struct {
    uint64_t dir_dst;
    uint64_t config_ptr;
  } arg;
  arg.dir_dst = (uint64_t) dst;
  arg.config_ptr = (uint64_t) config;

  // copy src to dst
#if defined(__linux__)
  MUST_OR_RET(visit_dir(src, vdcb, (void *) &arg) == 0, -1, "visit_dir failed: dst: %s, src: %s", dst, src);
#else
  UNIMPLEMENTED();
#endif

  return 0;
}
