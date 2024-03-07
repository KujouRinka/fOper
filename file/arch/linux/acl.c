#include "acl.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <grp.h>
#include <pwd.h>
#include <unistd.h>

#include <acl/libacl.h>
#include <sys/acl.h>
#include <sys/stat.h>

#include "common.h"

int file_acl_show(int fd) {
  acl_t acl = acl_get_fd(fd);
  if (acl == NULL) {
    return -1;
  }

  char *s;
  LOG("\033[33mACL: \n%s\033[0m", acl_to_text(acl, NULL));
  LOG("\033[33mACL entries: %d\033[0m", acl_entries(acl));

  acl_free(acl);
  return 0;
}

int file_acl_set_owner(int fd, const char *owner) {
  uid_t uid = get_uid(owner);
  MUST_OR_RET(uid != -1, -1, "invalid uid");
  MUST_OR_RET(fchown(fd, uid, -1) == 0, -1, "fchown failed: %s", strerror(errno));
  return 0;
}

int file_acl_set_group(int fd, const char *group) {
  gid_t gid = get_gid(group);
  MUST_OR_RET(gid != -1, -1, "invalid gid");
  MUST_OR_RET(fchown(fd, -1, gid) == 0, -1, "fchown failed: %s", strerror(errno));
  return 0;
}

int file_acl_set_mode(int fd, const char *perm) {
  MUST_OR_RET(strlen(perm) == 9, -1, "invalid perm: %s", perm);
  mode_t mode = 0;
  if (perm[0] == 'r') {
    mode |= S_IRUSR;
  }
  if (perm[1] == 'w') {
    mode |= S_IWUSR;
  }
  if (perm[2] == 'x') {
    mode |= S_IXUSR;
  }
  if (perm[3] == 'r') {
    mode |= S_IRGRP;
  }
  if (perm[4] == 'w') {
    mode |= S_IWGRP;
  }
  if (perm[5] == 'x') {
    mode |= S_IXGRP;
  }
  if (perm[6] == 'r') {
    mode |= S_IROTH;
  }
  if (perm[7] == 'w') {
    mode |= S_IWOTH;
  }
  if (perm[8] == 'x') {
    mode |= S_IXOTH;
  }
  MUST_OR_RET(fchmod(fd, mode) == 0, -1, "fchmod failed: %s", strerror(errno));
  return 0;
}

char *file_acl_get_owner(int fd, const char *owner) {
  UNIMPLEMENTED();
}

char *file_acl_get_group(int fd, const char *group) {
  UNIMPLEMENTED();
}

int file_acl_set_acl(int fd, const char *acl_str) {
  FN_META();

  acl_t acl = acl_from_text(acl_str);
  MUST_OR_RET(acl != NULL, -1, "acl_from_text failed: %s", strerror(errno));
  free_list_add(acl, acl_free);

  MUST_OR_FREE_RET(acl_valid(acl) == 0, -1, "acl_valid failed: %s", strerror(errno));
  MUST_OR_FREE_RET(acl_set_fd(fd, acl) == 0, -1, "acl_set_fd failed: %s", strerror(errno));

  FN_CLEAN();
  return 0;
}

int file_acl_add_entry(int fd, const char *entry_str) {
  FN_META();

  acl_t acl = acl_get_fd(fd);
  MUST_OR_RET(acl != NULL, -1, "acl_get_fd failed: %s", strerror(errno));
  free_list_add(acl, acl_free);

  acl_t new_acl = acl_from_text(entry_str);
  MUST_OR_FREE_RET(new_acl != NULL, -1, "acl_from_text failed: %s", strerror(errno));
  free_list_add(new_acl, acl_free);

  acl_entry_t entry;
  acl_entry_t new_entry;
  MUST_OR_FREE_RET(acl_create_entry(&acl, &entry) == 0, -1, "acl_create_entry failed: %s", strerror(errno));
  MUST_OR_FREE_RET(acl_get_entry(new_acl, 0, &new_entry) == 1, -1, "acl_copy_entry failed: %s", strerror(errno));
  MUST_OR_FREE_RET(acl_copy_entry(entry, new_entry) == 0, -1, "acl_copy_entry failed: %s", strerror(errno));
  MUST_OR_FREE_RET(acl_valid(acl) == 0, -1, "acl_valid failed: %s", strerror(errno));
  MUST_OR_FREE_RET(acl_set_fd(fd, acl) == 0, -1, "acl_set_fd failed: %s", strerror(errno));
  MUST_OR_FREE_RET(acl_calc_mask(&acl) == 0, -1, "acl_calc_mask failed: %s", strerror(errno));

  FN_CLEAN();

  return 0;
}

int file_acl_remove_entry(int fd, const char *entry_str) {
  UNIMPLEMENTED();
  return 0;
}

int file_acl_copy(int dst, int src) {
  FN_META();

  acl_t acl_src = acl_get_fd(src);
  MUST(acl_src != NULL, "acl_get_fd failed");

  MUST_OR_FREE_RET(acl_set_fd(dst, acl_src) == 0, -1, "acl_set_fd failed: %s", strerror(errno));

  FN_CLEAN();
  return 0;
}

static uid_t get_uid(const char *owner) {
  struct passwd *pwd = getpwnam(owner);
  if (pwd == NULL) {
    return -1;
  }
  return pwd->pw_uid;
}

static gid_t get_gid(const char *group) {
  struct group *grp = getgrnam(group);
  if (grp == NULL) {
    return -1;
  }
  return grp->gr_gid;
}
