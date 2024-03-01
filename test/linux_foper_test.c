#include <stdio.h>

#if defined(__linux__)
#include "arch/linux/linux_foper.h"
#include <fcntl.h>
#include <linux/fs.h>
#include <unistd.h>
#endif

#include "common.h"

int main() {
  LOG("\033[1;32mTest IOCTL Begin\033[0m\n");

#if defined(__linux__)
  int file = open("./config/config.yaml", O_RDONLY, 0666);
  unsigned long flags;
  MUST(file_get_attr(file, &flags) == 0, "file_get_attr failed");

  // add immutable flag

  unsigned long new_flags;
  MUST(file_add_attr(file, FS_IMMUTABLE_FL) == 0, "file_add_attr failed");
  MUST(file_get_attr(file, &new_flags) == 0, "file_get_attr failed");
  MUST(file_has_attr(file, FS_IMMUTABLE_FL) == 1, "file_has_attr failed");

  // restore flags
  MUST(file_ch_attr(file, flags) == 0, "file_set_attr failed");

  close(file);
#endif

  LOG("\033[1;32mTest IOCTL Passed!\033[0m\n");
  return 0;
}
