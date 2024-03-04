#if defined(__linux__)
#include "file/arch/linux/acl.h"

#include <unistd.h>
#include <fcntl.h>
#endif

#include "common.h"

#include <errno.h>

int main() {
  LOG("\033[1;32mTest foper acl Begin\033[0m");
#if defined(__linux__)
  int fd = open("./config/config.yaml", O_RDONLY);
  int fd2 = open("./config/config_copy.yaml", O_RDONLY);

  file_acl_show(fd);
  file_acl_show(fd2);

  // MUST(file_acl_add_entry(fd, "user:rinka:rw-") == 0, "file_acl_add_entry failed");
  MUST(file_acl_copy(fd2, fd) == 0, "file_acl_copy failed");
  file_acl_show(fd);
  file_acl_show(fd2);

  close(fd);
#endif
  LOG("\033[1;32mTest foper acl Passed!\033[0m");
  return 0;
}
