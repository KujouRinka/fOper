#ifndef CONFIG_H
#define CONFIG_H

#include <yaml.h>
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

struct Kv {
  char *key;
  char *value;
};

struct Operation {
  int kv_cnt;
  struct Kv *kvs;
};

struct Config *parse_config(const char *filename);
struct Operation *parse_operation(yaml_parser_t *parser, int *operation_count);
void free_config(struct Config *config);
char *OperationGet(struct Operation *op, const char *key);

#endif
