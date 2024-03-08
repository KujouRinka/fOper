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
  char *filename_dst = TRAIT(f_dst, FileTrait, filename);
  char *filename_src = TRAIT(f_src, FileTrait, filename);

  return fwrite_property(fd_dst, filename_dst, fd_src, filename_src, config);
}

int fwrite_property(int fd_dst, const char *filename_dst, int fd_src, const char *filename_src, struct ConfigObj *config) {
  Action perm_action, attr_action, xattr_action, acl_action, time_action;
  perm_action = attr_action = xattr_action = acl_action = time_action = Default;

  char *perm_owner, *perm_group, *perm_mode;
  char *attr_operation;
  struct ConfigObj *xattr_operation;
  char *acl_operation;

  char *time_fmt, *ctime, *atime, *mtime;

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

  // parse xattr properties
  cmd_obj = CONFIG_TRAIT(config, as_map, "xattr");
  if (cmd_obj != NULL) {
    action = config_map_get_string(cmd_obj, "action");
    MUST_OR_RET(action != NULL, -1, "action is null");
    xattr_action = str2action(action);

    if (xattr_action == Mod) {
      xattr_operation = CONFIG_TRAIT(cmd_obj, as_map, "operation");
      CONFIG_REQUIRE(xattr_operation, CONFIG_TYPE_ARRAY);
    }
  }

  // parse acl properties
  cmd_obj = CONFIG_TRAIT(config, as_map, "acl");
  if (cmd_obj != NULL) {
    action = config_map_get_string(cmd_obj, "action");
    MUST_OR_RET(action != NULL, -1, "action is null");
    acl_action = str2action(action);

    if (acl_action == Mod) {
      acl_operation = config_map_get_string(cmd_obj, "operation");
    }
  }

  // parse time properties
  cmd_obj = CONFIG_TRAIT(config, as_map, "time");
  if (cmd_obj != NULL) {
    action = config_map_get_string(cmd_obj, "action");
    MUST_OR_RET(action != NULL, -1, "action is null");
    time_action = str2action(action);

    if (time_action == Mod) {
      time_fmt = config_map_get_string(cmd_obj, "fmt");
      if (time_fmt == NULL) {
        time_fmt = "%Y%m%d %H%M%S";
      }
      ctime = config_map_get_string(cmd_obj, "ctime");
      atime = config_map_get_string(cmd_obj, "atime");
      mtime = config_map_get_string(cmd_obj, "mtime");
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

  // set xattr
  if (xattr_action == Mod) {
    for (size_t i = 0; i < CONFIG_TRAIT(xattr_operation, as_array_size); ++i) {
      struct ConfigObj *xattr = CONFIG_TRAIT(xattr_operation, as_array, i);
      char *verb = config_map_get_string(xattr, "verb");
      MUST_OR_RET(verb != NULL, -1, "verb is null");

      char *name, *value;
      if (strcmp(verb, "set") == 0) {
        name = config_map_get_string(xattr, "name");
        value = config_map_get_string(xattr, "value");
        MUST_OR_RET(name != NULL, -1, "name is null");
        MUST_OR_RET(value != NULL, -1, "value is null");
        MUST_OR_RET(file_set_xattr(fd_dst, name, value) == 0, -1, "file_set_xattr failed");
      } else if (strcmp(verb, "del") == 0) {
        name = config_map_get_string(xattr, "name");
        MUST_OR_RET(name != NULL, -1, "name is null");
        MUST_OR_RET(file_remove_xattr(fd_dst, name) == 0, -1, "file_del_xattr failed");
      } else {
        FATAL("Unknown verb: %s\n", verb);
      }
    }
  } else if (xattr_action == Copy) {
    MUST_OR_RET(file_copy_xattr(fd_dst, fd_src) == 0, -1, "file_copy_xattr failed");
  }

  // set acl
  if (acl_action == Mod) {
    MUST_OR_RET(file_acl_set_acl(fd_dst, acl_operation) == 0, -1, "file_set_acl failed");
  } else if (acl_action == Copy) {
    MUST_OR_RET(file_acl_copy(fd_dst, fd_src) == 0, -1, "file_copy_acl failed");
  }

  // set attr
  if (attr_action == Mod) {
    MUST_OR_RET(file_set_attr(fd_dst, attr_operation, fd_src) == 0, -1, "file_set_attr failed");
  } else if (attr_action == Copy) {
    MUST_OR_RET(file_copy_attr(fd_dst, fd_src) == 0, -1, "file_copy_attr failed");
  }

  // set time
  if (time_action == Mod && filename_dst != NULL) {
    if (ctime != NULL) {
      MUST_OR_RET(file_set_ctime(filename_dst, ctime, time_fmt) == 0, -1, "file_set_time failed");
    }
    if (atime != NULL) {
      MUST_OR_RET(file_set_atime(filename_dst, atime, time_fmt) == 0, -1, "file_set_time failed");
    }
    if (mtime != NULL) {
      MUST_OR_RET(file_set_mtime(filename_dst, mtime, time_fmt) == 0, -1, "file_set_time failed");
    }
  } else if (time_action == Copy && filename_dst && filename_src) {
    MUST_OR_RET(file_copy_times(filename_dst, filename_src) == 0, -1, "file_copy_times failed");
  }
#else
  UNIMPLEMENTED();
#endif

  return 0;
}

int fnwrite_property(const char *dst, const char *src, struct ConfigObj *config) {
  FN_META();

  int ret = 0;
#if defined(__linux__)
  int fd_dst = open(dst, O_RDWR);
  MUST_OR_FREE_RET(fd_dst >= 0, -1, "open failed: %s", strerror(errno));
  free_list_add((void *) ((uint64_t) fd_dst), (int (*)(void *)) close);
  int fd_src = open(src, O_RDONLY);
  MUST_OR_FREE_RET(fd_src >= 0, -1, "open failed: %s", strerror(errno));
  free_list_add((void *) ((uint64_t) fd_src), (int (*)(void *)) close);

  ret = fwrite_property(fd_dst, dst, fd_src, src, config);

#else
  UNIMPLEMENTED();
#endif

  FN_CLEAN();
  return ret;
}

int fnwrite_dir_property(const char *dst, const char *src, struct ConfigObj *config) {
  FN_META();

  int ret = -1;
#if defined(__linux__)
  // open dir and get fd
  DIR *dst_dir, *src_dir;
  dst_dir = opendir(dst);
  MUST_OR_FREE_RET(dst_dir != NULL, -1, "opendir failed: %s", strerror(errno));
  free_list_add(dst_dir, (int (*)(void *)) closedir);
  src_dir = opendir(src);
  MUST_OR_FREE_RET(src_dir != NULL, -1, "opendir failed: %s", strerror(errno));
  free_list_add(src_dir, (int (*)(void *)) closedir);

  int fd_dst = dirfd(dst_dir);
  int fd_src = dirfd(src_dir);

  // write property
  ret = fwrite_property(fd_dst, dst, fd_src, src, config);

#else
  UNIMPLEMENTED();
#endif

  FN_CLEAN();
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
