#ifndef FILE_LINUX_ACL_H
#define FILE_LINUX_ACL_H

#include <sys/acl.h>

int file_acl_set_owner(int fd, const char *owner);
int file_acl_set_group(int fd, const char *group);
int file_acl_set_mode(int fd, const char *perm);
char *file_acl_get_owner(int fd, const char *owner);
char *file_acl_get_group(int fd, const char *group);

int file_acl_set_acl(int fd, const char *acl_str);
int file_acl_add_entry(int fd, const char *entry_str);
int file_acl_remove_entry(int fd, const char *entry_str);
int file_acl_copy(int dst, int src);

int file_acl_show(int fd);

static uid_t get_uid(const char *owner);
static gid_t get_gid(const char *group);

#endif
