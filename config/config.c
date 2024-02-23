#include "config.h"

#include "yaml.h"
#include "common.h"

struct Config *parse_config(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Failed to open file.\n");
    return NULL;
  }

  yaml_parser_t parser;
  yaml_event_t event;

  struct Config *config = malloc(sizeof(struct Config));
  if (!config) {
    fprintf(stderr, "Failed to allocate memory for config.\n");
    return NULL;
  }
  memset(config, 0, sizeof(struct Config));

  yaml_parser_initialize(&parser);
  yaml_parser_set_input_file(&parser, file);

  while (1) {
    if (!yaml_parser_parse(&parser, &event)) {
      fprintf(stderr, "Failed to parse YAML.\n");
      free_config(config);
      return NULL;
    }

    if (event.type == YAML_SCALAR_EVENT) {
      if (strcmp((char *) event.data.scalar.value, "gen-file") == 0) {
        yaml_parser_parse(&parser, &event);
        config->gen_filename = strdup((char *) event.data.scalar.value);
      } else if (strcmp((char *) event.data.scalar.value, "external-file") == 0) {
        yaml_event_t next_event;
        yaml_parser_parse(&parser, &next_event);

        if (next_event.type == YAML_SEQUENCE_START_EVENT) {
          yaml_event_t e2;
          yaml_parser_parse(&parser, &e2);

          int cap = 10;
          config->extern_files = malloc(sizeof(char *) * cap);
          memset(config->extern_files, 0, sizeof(char *) * cap);
          while (e2.type != YAML_SEQUENCE_END_EVENT) {
            if (config->extern_file_size == cap) {
              cap *= 2;
              config->extern_files = realloc(config->extern_files, sizeof(char *) * cap);
              if (!config->extern_files) {
                fprintf(stderr, "Failed to allocate memory for extern_files.\n");
                free_config(config);
                return NULL;
              }
            }
            memset(
                &config->extern_files[config->extern_file_size],
                0,
                sizeof(char *) * (cap - config->extern_file_size)
            );
            if (e2.type == YAML_SCALAR_EVENT) {
              config->extern_files[config->extern_file_size++] = strdup((char *) e2.data.scalar.value);
            }
            yaml_event_delete(&e2);
            yaml_parser_parse(&parser, &e2);
          }
          yaml_event_delete(&e2);
        }
        yaml_event_delete(&next_event);
      } else if (strcmp((char *) event.data.scalar.value, "create-method") == 0) {
        yaml_parser_parse(&parser, &event);
        config->create_method = strdup((char *) event.data.scalar.value);
      } else if (strcmp((char *) event.data.scalar.value, "operation") == 0) {
        config->operations = parse_operation(&parser, &config->operation_count);
      }
    }

    enum yaml_event_type_e type = event.type;
    yaml_event_delete(&event);

    if (type == YAML_STREAM_END_EVENT) {
      break;
    }
  }

  fclose(file);
  yaml_parser_delete(&parser);

  return config;
}

struct Operation *parse_operation(yaml_parser_t *parser, int *operation_count) {
  struct Operation *operations = NULL;
  yaml_event_t event;
  yaml_parser_parse(parser, &event);

  int operation_index = 0;

  enum yaml_event_type_e type = event.type;
  yaml_event_delete(&event);
  if (type == YAML_SEQUENCE_START_EVENT) {
    yaml_parser_parse(parser, &event);

    int cap = 10;
    operations = malloc(sizeof(struct Operation) * cap);
    if (operations == NULL) {
      EPRINTF("Failed to allocate memory for operations.");
      return NULL;
    }
    memset(operations, 0, sizeof(struct Operation) * cap);
    type = event.type;
    yaml_event_delete(&event);
    while (type != YAML_SEQUENCE_END_EVENT) {
      if (type == YAML_MAPPING_START_EVENT) {
        if (operation_index == cap) {
          cap *= 2;
          operations = realloc(operations, sizeof(struct Operation) * cap);
          if (!operations) {
            EPRINTF("Failed to allocate memory for operations.");
            return NULL;
          }
        }
        memset(
            &operations[operation_index],
            0,
            sizeof(struct Operation) * (cap - operation_index)
        );
        struct Operation *operation = &operations[operation_index++];
        yaml_parser_parse(parser, &event);

        int kv_cap = 10;
        operation->kvs = malloc(sizeof(struct Kv) * kv_cap);
        memset(operation->kvs, 0, sizeof(struct Kv) * kv_cap);
        operation->kv_cnt = 0;
        while (event.type != YAML_MAPPING_END_EVENT) {
          if (operation->kv_cnt == kv_cap) {
            kv_cap *= 2;
            operation->kvs = realloc(operation->kvs, sizeof(struct Kv) * kv_cap);
            if (!operation->kvs) {
              EPRINTF("Failed to allocate memory for kvs.");
              return NULL;
            }
          }
          memset(
              &operation->kvs[operation->kv_cnt],
              0,
              sizeof(struct Kv) * (kv_cap - operation->kv_cnt)
          );
          struct Kv *kv = &operation->kvs[operation->kv_cnt++];
          kv->key = strdup((char *) event.data.scalar.value);
          yaml_event_delete(&event);
          yaml_parser_parse(parser, &event);
          kv->value = strdup((char *) event.data.scalar.value);
          yaml_event_delete(&event);

          yaml_parser_parse(parser, &event);
        }
      }
      yaml_event_delete(&event);
      yaml_parser_parse(parser, &event);
      type = event.type;
    }
    *operation_count = operation_index;
  } else {
    EPRINTF("Invalid operation.\n");
    return NULL;
  }
  yaml_event_delete(&event);
  return operations;
}

void free_config(struct Config *config) {
  if (config->gen_filename) {
    free(config->gen_filename);
  }

  if (config->extern_files) {
    for (int i = 0; i < config->extern_file_size; i++) {
      free(config->extern_files[i]);
    }
    free(config->extern_files);
  }

  if (config->create_method) {
    free(config->create_method);
  }

  if (config->operations) {
    for (int i = 0; i < config->operation_count; i++) {
      for (int j = 0; j < config->operations[i].kv_cnt; j++) {
        free(config->operations[i].kvs[j].key);
        free(config->operations[i].kvs[j].value);
      }
      free(config->operations[i].kvs);
    }
    free(config->operations);
  }

  free(config);
}

char *OperationGet(struct Operation *op, const char *key) {
  for (int i = 0; i < op->kv_cnt; i++) {
    if (strcmp(op->kvs[i].key, key) == 0) {
      return op->kvs[i].value;
    }
  }
  return "";
}
