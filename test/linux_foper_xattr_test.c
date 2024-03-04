#if defined(__linux__)
#include "file/arch/linux/foper.h"

#include <fcntl.h>
#include <unistd.h>
#endif

#include "common.h"

int main() {
  LOG("\033[1;32mTest foper xattr Begin\033[0m");
#if defined(__linux__)
  int fd = open("./config/config.yaml", O_RDONLY);

  LOG("\033[33mSetting file xattr...\033[0m");

  LOG("\033[33mSetting user.foo: bar\033[0m");
  MUST(file_set_xattr(fd, "user.foo", "bar") == 0, "file_set_xattr failed");

  LOG("\033[33mSetting user.what: hy\033[0m");
  MUST(file_set_xattr(fd, "user.what", "hy") == 0, "file_set_xattr failed");

  LOG("\033[33mSetting user.42: 233\033[0m");
  MUST(file_set_xattr(fd, "user.42", "233") == 0, "file_set_xattr failed");

  LOG("\033[33mSetting user.114514: 1919810\033[0m");
  MUST(file_set_xattr(fd, "user.114514", "1919810") == 0, "file_set_xattr failed");

  LOG("\033[33mFile xattr set OK\033[0m");

  char *xattr_list = NULL;
  ssize_t xattr_list_len = 0;
  LOG("\033[33mGetting file xattr\033[0m");
  MUST(file_list_xattr(fd, &xattr_list, &xattr_list_len) == 0, "file_list_xattr failed");
  LOG("\033[33mFile has xattr: \033[0m");
  file_log_xattr(fd);

  MUST(file_has_xattr(fd, "user.foo", "bar") == 1, "file should have xattr user.foo=bar but not");
  MUST(file_has_xattr(fd, "user.what", "hy") == 1, "file should have xattr user.what=hy but not");
  MUST(file_has_xattr(fd, "user.42", "233") == 1, "file should have xattr user.42=233 but not");
  MUST(file_has_xattr(fd, "user.114514", "1919810") == 1, "file should have xattr user.114514=1919810 but not");

  LOG("\033[33mRemoving file xattr\033[0m");
  MUST(file_remove_xattr(fd, "user.foo") == 0, "file_remove_xattr failed");
  MUST(file_remove_xattr(fd, "user.what") == 0, "file_remove_xattr failed");
  MUST(file_remove_xattr(fd, "user.42") == 0, "file_remove_xattr failed");
  MUST(file_remove_xattr(fd, "user.114514") == 0, "file_remove_xattr failed");

  LOG("\033[33mFile xattr removed OK\033[0m");

  MUST(file_has_xattr(fd, "user.foo", "bar") == 0, "file should not have xattr user.foo=bar but has");
  MUST(file_has_xattr(fd, "user.what", "hy") == 0, "file should not have xattr user.what=hy but has");
  MUST(file_has_xattr(fd, "user.42", "233") == 0, "file should not have xattr user.42=233 but has");
  MUST(file_has_xattr(fd, "user.114514", "1919810") == 0, "file should not have xattr user.114514=1919810 but has");

  close(fd);
#endif

  LOG("\033[1;32mTest foper xattr Passed!\033[0m\n");
  return 0;
}
