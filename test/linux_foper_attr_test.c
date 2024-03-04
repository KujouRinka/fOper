#if defined(__linux__)
#include "file/arch/linux/foper.h"

#include <fcntl.h>
#include <linux/fs.h>
#include <unistd.h>
#endif

#include "common.h"

int main() {
  LOG("\033[1;32mTest foper attr Begin\033[0m");

#if defined(__linux__)
  int file = open("./config/config.yaml", O_RDONLY, 0666);
  unsigned long flags;
  MUST(file_get_attr(file, &flags) == 0, "file_get_attr failed");

  // add immutable flag
  LOG("\033[33mAdd immutable flag...\033[0m");

  unsigned long new_flags;
  MUST(file_add_attr(file, FS_IMMUTABLE_FL) == 0, "file_add_attr failed");
  LOG("\033[33mCheck immutable flag...\033[0m");
  MUST(file_get_attr(file, &new_flags) == 0, "file_get_attr failed");
  MUST(file_has_attr(file, FS_IMMUTABLE_FL) == 1, "file_has_attr failed");

  LOG("\033[33mRestoring flag...\033[0m");
  // restore flags
  MUST(file_ch_attr(file, flags) == 0, "file_set_attr failed");
  LOG("\033[33mOK\033[0m");

  close(file);
#endif

  LOG("\033[1;32mTest foper attr Passed!\033[0m");
  return 0;
}
