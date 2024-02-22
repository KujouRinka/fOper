#ifndef CONFIG_H
#define CONFIG_H

#define MAX_FILENAME_LEN 256

struct Operation;

struct Config {
  char *gen_filename;

  int extern_file_size;
  char **extern_files;

  char *create_method;

  int operation_count;
  struct Operation *operations;
};

struct Operation {
  char *action;
  char *verb;
  char *par1;
  char *par2;
  char *par3;
};

struct Config *parse_config(const char *filename);
void free_config(struct Config *config);

#endif
