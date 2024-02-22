#include "config.h"

#include "yaml.h"

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

  int operation_index = 0;

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
        // TODO: mem leak here
        yaml_event_t next_event;
        yaml_parser_parse(&parser, &next_event);

        if (next_event.type == YAML_SEQUENCE_START_EVENT) {
          yaml_parser_parse(&parser, &next_event);

          int cap = 10;
          config->operations = malloc(sizeof(struct Operation) * cap);
          memset(config->operations, 0, sizeof(struct Operation) * cap);
          while (next_event.type != YAML_SEQUENCE_END_EVENT) {
            if (next_event.type == YAML_MAPPING_START_EVENT) {
              if (operation_index == cap) {
                cap *= 2;
                config->operations = realloc(config->operations, sizeof(struct Operation) * cap);
                if (!config->operations) {
                  fprintf(stderr, "Failed to allocate memory for operations.\n");
                  free_config(config);
                  return NULL;
                }
              }
              memset(
                  &config->operations[operation_index],
                  0,
                  sizeof(struct Operation) * (cap - operation_index)
              );
              struct Operation *operation = &config->operations[operation_index++];
              yaml_parser_parse(&parser, &next_event);
              while (next_event.type != YAML_MAPPING_END_EVENT) {

                if (strcmp((char *) next_event.data.scalar.value, "action") == 0) {
                  yaml_parser_parse(&parser, &next_event);
                  operation->action = strdup((char *) next_event.data.scalar.value);
                } else if (strcmp((char *) next_event.data.scalar.value, "verb") == 0) {
                  yaml_parser_parse(&parser, &next_event);
                  operation->verb = strdup((char *) next_event.data.scalar.value);
                } else if (strcmp((char *) next_event.data.scalar.value, "par1") == 0) {
                  yaml_parser_parse(&parser, &next_event);
                  operation->par1 = strdup((char *) next_event.data.scalar.value);
                } else if (strcmp((char *) next_event.data.scalar.value, "par2") == 0) {
                  yaml_parser_parse(&parser, &next_event);
                  operation->par2 = strdup((char *) next_event.data.scalar.value);
                } else if (strcmp((char *) next_event.data.scalar.value, "par3") == 0) {
                  yaml_parser_parse(&parser, &next_event);
                  operation->par3 = strdup((char *) next_event.data.scalar.value);
                }

                yaml_event_delete(&next_event);
                yaml_parser_parse(&parser, &next_event);
              }
            }
            yaml_event_delete(&next_event);
            yaml_parser_parse(&parser, &next_event);
          }
          config->operation_count = operation_index;
        }
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
      free(config->operations[i].action);
      free(config->operations[i].verb);
      free(config->operations[i].par1);
      free(config->operations[i].par2);
      free(config->operations[i].par3);
    }
    free(config->operations);
  }

  free(config);
}
