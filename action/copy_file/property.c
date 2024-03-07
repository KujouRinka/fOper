#include "property.h"

#include <errno.h>

#if defined(__linux__)
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file/arch/linux/foper.h"
#endif

#include "common.h"
#include "config/config.h"
#include "copy_file.h"
#include "file/trait.h"

int write_property(struct FileTrait **f_dst, struct FileTrait **f_src, struct ConfigObj *config) {
  int fd_dst = TRAIT(f_dst, FileTrait, fd);
  int fd_src = TRAIT(f_src, FileTrait, fd);

  return fwrite_property(fd_dst, fd_src, config);
}

int fwrite_property(int fd_dst, int fd_src, struct ConfigObj *config) {
  // read config information

  Action perm_action, attr_action, xattr_action, acl_action;
  perm_action = attr_action = xattr_action = acl_action = Default;

  char *perm_owner, *perm_group, *perm_mode;
  char *attr_operation;
  struct ConfigObj *xattr_operation;
  char *acl_operation;

  struct ConfigObj *action_obj, *cmd_obj;
  char *action;

  // parse permission properties
  cmd_obj = CONFIG_TRAIT(config, as_map, "permission");
  if (cmd_obj != NULL) {
    action = config_map_get_string(cmd_obj, "action");
    MUST_OR_RET(action != NULL, -1, "action is null");
    perm_action = str2action(action);

    if (perm_action == Mod) {
      perm_owner = config_map_get_string(cmd_obj, "owner");
      perm_group = config_map_get_string(cmd_obj, "group");
      perm_mode = config_map_get_string(cmd_obj, "mode");
    }
  }

  // parse attr properties
  cmd_obj = CONFIG_TRAIT(config, as_map, "attr");
  if (cmd_obj != NULL) {
    action = config_map_get_string(cmd_obj, "action");
    MUST_OR_RET(action != NULL, -1, "action is null");
    attr_action = str2action(action);

    if (attr_action == Mod) {
      attr_operation = config_map_get_string(cmd_obj, "operation");
    }
  }

#if defined(__linux__)
  // set file properties

  // set perm
  if (perm_action == Mod) {
    if (perm_owner != NULL) {
      MUST_OR_RET(file_acl_set_owner(fd_dst, perm_owner) == 0, -1, "file_acl_set_owner failed");
    }
    if (perm_group != NULL) {
      MUST_OR_RET(file_acl_set_group(fd_dst, perm_group) == 0, -1, "file_acl_set_group failed");
    }
    if (perm_mode != NULL) {
      MUST_OR_RET(file_acl_set_mode(fd_dst, perm_mode) == 0, -1, "file_acl_set_mode failed");
    }
  } else if (perm_action == Copy) {
    struct stat st_dst;
    struct stat st_src;
    MUST_OR_RET(fstat(fd_dst, &st_dst) == 0, -1, "fstat failed");
    MUST_OR_RET(fstat(fd_src, &st_src) == 0, -1, "fstat failed");
    fchown(fd_dst, st_src.st_uid, st_src.st_gid);
    fchmod(fd_dst, st_src.st_mode);
  }

  // set attr
  if (attr_action == Mod) {
    MUST_OR_RET(file_set_attr(fd_dst, attr_operation, fd_src) == 0, -1, "file_set_attr failed");
  } else if (attr_action == Copy) {
    MUST_OR_RET(file_copy_attr(fd_dst, fd_src) == 0, -1, "file_copy_attr failed");
  }
#else
  UNIMPLEMENTED();
#endif

  return 0;
}

int fnwrite_property(const char *dst, const char *src, struct ConfigObj *config) {
  int ret = 0;
#if defined(__linux__)
  int fd_dst = open(dst, O_RDWR);
  MUST_OR_RET(fd_dst >= 0, -1, "open failed: %s", strerror(errno));
  int fd_src = open(src, O_RDONLY);
  MUST_OR_RET(fd_src >= 0, -1, "open failed: %s", strerror(errno));

  ret = fwrite_property(fd_dst, fd_src, config);

  close(fd_dst);
  close(fd_src);
#else
  UNIMPLEMENTED();
#endif

  return ret;
}

int fnwrite_dir_property(const char *dst, const char *src, struct ConfigObj *config) {
  int ret = -1;
#if defined(__linux__)
  // open dir and get fd
  DIR *dst_dir, *src_dir;
  dst_dir = opendir(dst);
  MUST_OR_RET(dst_dir != NULL, -1, "opendir failed: %s", strerror(errno));
  src_dir = opendir(src);
  MUST_OR_RET(src_dir != NULL, -1, "opendir failed: %s", strerror(errno));

  int fd_dst = dirfd(dst_dir);
  int fd_src = dirfd(src_dir);

  // write property
  ret = fwrite_property(fd_dst, fd_src, config);

  closedir(dst_dir);
  closedir(src_dir);
#else
  UNIMPLEMENTED();
#endif
  return ret;
}

static Action str2action(const char *action) {
  if (strcmp(action, "default") == 0) {
    return Default;
  } else if (strcmp(action, "mod") == 0) {
    return Mod;
  } else if (strcmp(action, "copy") == 0) {
    return Copy;
  } else {
    FATAL("Unknown action: %s\n", action);
  }
}
