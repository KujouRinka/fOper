#include "config.h"

#include <stdlib.h>
#include <string.h>

#include <yaml.h>

#include "common.h"

struct ConfigObj *parse_config_v2(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return NULL;
  }

  yaml_parser_t parser;
  if (!yaml_parser_initialize(&parser)) {
    fclose(file);
    return NULL;
  }
  yaml_parser_set_input_file(&parser, file);

  return parsing_driver(&parser);
}

struct ConfigObjTrait defaultConfigTrait = {
    .type = config_obj_type_default,
    .as_int = config_obj_as_int_default,
    .as_float = config_obj_as_float_default,
    .as_byte = config_obj_as_byte_default,
    .as_string = config_obj_as_string_default,
    .as_bool = config_obj_as_bool_default,
    .as_map = config_obj_as_map_default,
    .as_map_by_index = config_obj_as_map_by_index_default,
    .as_map_size = config_obj_as_map_size_default,
    .as_array = config_obj_as_array_default,
    .as_array_size = config_obj_as_array_size_default,

    .free = config_obj_free_default,
};

enum ConfigType config_obj_type_default(void *self) {
  struct ConfigObj *obj = self;
  return obj->type;
}

int config_obj_as_int_default(void *self) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_BYTE);
  struct ConfigByte *obj = self;
  char *end;
  int ret = strtol((char *) obj->data, &end, 10);
  MUST(*end == '\0', "Invalid number: %s", (char *) obj->data);
  return ret;
}

float config_obj_as_float_default(void *self) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_FLOAT);
  struct ConfigByte *obj = self;
  char *end;
  float ret = strtof((char *) obj->data, &end);
  MUST(*end == '\0', "Invalid number: %s", (char *) obj->data);
  return ret;
}

struct ConfigByte *config_obj_as_byte_default(void *self) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_BYTE);
  struct ConfigByte *obj = self;
  return obj;
}

static char *config_obj_as_string_default(void *self) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_BYTE);
  struct ConfigByte *obj = self;
  return (char *) obj->data;
}

int config_obj_as_bool_default(void *self) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_BOOL);
  struct ConfigBool *obj = self;
  return obj->value;
}

struct ConfigObj *config_obj_as_map_default(void *self, const char *key) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_MAP);
  struct ConfigMap *obj = self;
  for (int i = 0; i < obj->len; ++i) {
    if (strcmp(obj->kvs[i].key, key) == 0) {
      return obj->kvs[i].value;
    }
  }
  return NULL;
}

static struct objKv *config_obj_as_map_by_index_default(void *self, int index) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_MAP);
  struct ConfigMap *obj = self;
  return &obj->kvs[index];
}

static size_t config_obj_as_map_size_default(void *self) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_MAP);
  struct ConfigMap *obj = self;
  return obj->len;
}

struct ConfigObj *config_obj_as_array_default(void *self, int index) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_ARRAY);
  struct ConfigArray *obj = self;
  return obj->data[index];
}

size_t config_obj_as_array_size_default(void *self) {
  CONFIG_REQUIRE(self, CONFIG_TYPE_ARRAY);
  struct ConfigArray *obj = self;
  return obj->len;
}

int config_obj_free_default(void *self) {
  return 0;
}

/*
 * Init function for each type
 */

struct ConfigInt *config_int_init(struct ConfigInt *c) {
  c->obj.trait = &defaultConfigTrait;
  c->obj.type = CONFIG_TYPE_INT;
  c->value = 0;
  return c;
}

struct ConfigFloat *config_float_init(struct ConfigFloat *c) {
  c->obj.trait = &defaultConfigTrait;
  c->obj.type = CONFIG_TYPE_FLOAT;
  c->value = 0;
  return c;
}

struct ConfigByte *config_string_init(struct ConfigByte *c) {
  c->obj.trait = &defaultConfigTrait;
  c->obj.type = CONFIG_TYPE_BYTE;
  c->data = NULL;
  c->len = 0;
  return c;
}

struct ConfigBool *config_bool_init(struct ConfigBool *c) {
  c->obj.trait = &defaultConfigTrait;
  c->obj.type = CONFIG_TYPE_BOOL;
  c->value = 0;
  return c;
}

struct objKv *config_kv_init(struct objKv *c) {
  c->key = NULL;
  c->value = NULL;
  return c;
}

struct ConfigMap *config_map_init(struct ConfigMap *c) {
  c->obj.trait = &defaultConfigTrait;
  c->obj.type = CONFIG_TYPE_MAP;
  c->kvs = NULL;
  c->len = 0;
  c->cap = 0;
  return c;
}

struct ConfigArray *config_array_init(struct ConfigArray *c) {
  c->obj.trait = &defaultConfigTrait;
  c->obj.type = CONFIG_TYPE_ARRAY;
  c->data = NULL;
  c->len = 0;
  c->cap = 0;
  return c;
}

/*
 * Parse function for each type
 */

static struct ConfigObj *parsing_driver(yaml_parser_t *parser) {
  struct ConfigObj *obj = NULL;

  yaml_token_t token;
  yaml_parser_scan(parser, &token);
  switch (token.type) {
    /* Stream start/end */
    case YAML_STREAM_START_TOKEN:
      obj = parsing_driver(parser);
      break;
    case YAML_STREAM_END_TOKEN:
      break;
    /* Token types (read before actual token) */
    case YAML_KEY_TOKEN:
      break;
    case YAML_VALUE_TOKEN:
      break;
    /* Block delimeters */
    case YAML_BLOCK_SEQUENCE_START_TOKEN:
      obj = config_obj_parse_array(parser);
      break;
    case YAML_BLOCK_ENTRY_TOKEN:
      break;
    case YAML_BLOCK_END_TOKEN:
      break;
    /* Data */
    case YAML_BLOCK_MAPPING_START_TOKEN:
      obj = config_obj_parse_map(parser);
      break;
    case YAML_SCALAR_TOKEN: {
      struct ConfigByte *cs = malloc(sizeof(struct ConfigByte));
      if (cs == NULL) {
        EPRINTF("malloc failed.\n");
        return NULL;
      }
      config_string_init(cs);
      cs->len = strlen((char *) token.data.scalar.value);
      cs->data = malloc(cs->len);
      if (cs->data == NULL) {
        EPRINTF("malloc failed.\n");
        free(cs);
        return NULL;
      }
      memcpy(cs->data, token.data.scalar.value, cs->len);
      cs->data[cs->len] = '\0';
      obj = (struct ConfigObj *) cs;
    } break;
    /* Others */
    default:
      EPRINTF("Unknown token type: %d\n", token.type);
      break;
  }
  yaml_token_delete(&token);

  return obj;
}

struct ConfigObj *config_obj_parse_int(yaml_parser_t *parser) {
  UNIMPLEMENTED();
}

struct ConfigObj *config_obj_parse_float(yaml_parser_t *parser) {
  UNIMPLEMENTED();
}

struct ConfigObj *config_obj_parse_byte(yaml_parser_t *parser) {
  UNIMPLEMENTED();
}

struct ConfigObj *config_obj_parse_bool(yaml_parser_t *parser) {
  UNIMPLEMENTED();
}

struct ConfigObj *config_obj_parse_map(yaml_parser_t *parser) {
  yaml_token_t token;
  struct ConfigMap *obj = malloc(sizeof(struct ConfigMap));
  if (obj == NULL) {
    EPRINTF("malloc failed.\n");
    return NULL;
  }
  config_map_init(obj);

  while (1) {
    // alloc kv
    struct objKv kv;
    config_kv_init(&kv);

    // read key
    yaml_parser_scan(parser, &token);
    if (token.type == YAML_BLOCK_END_TOKEN) {
      yaml_token_delete(&token);
      break;
    } else if (token.type != YAML_KEY_TOKEN) {
      EPRINTF("Invalid token type: %d\n", token.type);
      return NULL;
    }
    yaml_token_delete(&token);
    yaml_parser_scan(parser, &token);
    if (token.type != YAML_SCALAR_TOKEN) {
      EPRINTF("Invalid token type: %d\n", token.type);
      return NULL;
    }
    kv.key = malloc(token.data.scalar.length + 1);
    if (kv.key == NULL) {
      EPRINTF("malloc failed.\n");
      yaml_token_delete(&token);
      return NULL;
    }
    memcpy(kv.key, token.data.scalar.value, token.data.scalar.length);
    kv.key[token.data.scalar.length] = '\0';

    // read value
    yaml_parser_scan(parser, &token);
    if (token.type != YAML_VALUE_TOKEN) {
      EPRINTF("Invalid token type: %d\n", token.type);
      free(kv.key);
      return NULL;
    }
    yaml_token_delete(&token);
    kv.value = parsing_driver(parser);
    append_to_arr((void **) &obj->kvs, &obj->len, &obj->cap, &kv, sizeof(struct objKv));
  }

  return (struct ConfigObj *) obj;
}

struct ConfigObj *config_obj_parse_array(yaml_parser_t *parser) {
  yaml_token_t token;
  struct ConfigArray *obj = malloc(sizeof(struct ConfigArray));
  if (obj == NULL) {
    EPRINTF("malloc failed.\n");
    return NULL;
  }
  config_array_init(obj);

  while (1) {
    yaml_parser_scan(parser, &token);
    if (token.type == YAML_BLOCK_END_TOKEN) {
      yaml_token_delete(&token);
      break;
    } else if (token.type != YAML_BLOCK_ENTRY_TOKEN) {
      EPRINTF("Invalid token type: %d\n", token.type);
      return NULL;
    }
    yaml_token_delete(&token);
    struct ConfigObj *value = parsing_driver(parser);
    append_to_arr((void **) &obj->data, &obj->len, &obj->cap, &value, sizeof(struct ConfigObj *));
  }

  return (struct ConfigObj *) obj;
}
