#ifndef CONFIG_INNER_H
#define CONFIG_INNER_H

#include <stddef.h>
#include <stdint.h>
#include <yaml.h>

#include "common.h"

enum ConfigType {
  CONFIG_TYPE_NULL = 0,
  CONFIG_TYPE_INT,
  CONFIG_TYPE_FLOAT,
  CONFIG_TYPE_BYTE,
  CONFIG_TYPE_BOOL,
  CONFIG_TYPE_MAP,
  CONFIG_TYPE_ARRAY,
};

struct ConfigObj;
struct ConfigInt;
struct ConfigFloat;
struct ConfigByte;
struct ConfigBool;
struct ConfigMap;
struct ConfigArray;

struct objKv;

struct ConfigObj *parse_config_v2(const char *filename);

struct ConfigObjTrait {
  enum ConfigType (*type)(void *self);
  int (*as_int)(void *self);
  float (*as_float)(void *self);
  struct ConfigByte *(*as_byte)(void *self);
  char *(*as_string)(void *self);
  int (*as_bool)(void *self);
  struct ConfigObj *(*as_map)(void *self, const char *key);
  struct objKv *(*as_map_by_index)(void *self, int index);
  size_t (*as_map_size)(void *self);
  struct ConfigObj *(*as_array)(void *self, int index);
  size_t (*as_array_size)(void *self);

  int (*free)(void *self);
};

extern struct ConfigObjTrait defaultConfigTrait;

static enum ConfigType config_obj_type_default(void *self);
static int config_obj_as_int_default(void *self);
static float config_obj_as_float_default(void *self);
static struct ConfigByte *config_obj_as_byte_default(void *self);
static char *config_obj_as_string_default(void *self);
static int config_obj_as_bool_default(void *self);
static struct ConfigObj *config_obj_as_map_default(void *self, const char *key);
static struct objKv *config_obj_as_map_by_index_default(void *self, int index);
static size_t config_obj_as_map_size_default(void *self);
static struct ConfigObj *config_obj_as_array_default(void *self, int index);
static size_t config_obj_as_array_size_default(void *self);
static int config_obj_free_default(void *self);

/*
 * Parse function for each type
 */
static struct ConfigObj *parsing_driver(yaml_parser_t *parser);
static struct ConfigObj *config_obj_parse_int(yaml_parser_t *parser);
static struct ConfigObj *config_obj_parse_float(yaml_parser_t *parser);
static struct ConfigObj *config_obj_parse_byte(yaml_parser_t *parser);
static struct ConfigObj *config_obj_parse_bool(yaml_parser_t *parser);
static struct ConfigObj *config_obj_parse_map(yaml_parser_t *parser);
static struct ConfigObj *config_obj_parse_array(yaml_parser_t *parser);

struct ConfigObj {
  struct ConfigObjTrait *trait;
  enum ConfigType type;
};

struct ConfigInt {
  struct ConfigObj obj;
  int value;
};

struct ConfigInt *config_int_init(struct ConfigInt *c);

struct ConfigFloat {
  struct ConfigObj obj;
  float value;
};

struct ConfigFloat *config_float_init(struct ConfigFloat *c);

struct ConfigByte {
  struct ConfigObj obj;
  uint8_t *data;
  size_t len;
};

struct ConfigByte *config_string_init(struct ConfigByte *c);

struct ConfigBool {
  struct ConfigObj obj;
  int value;
};

struct ConfigBool *config_bool_init(struct ConfigBool *c);

struct objKv {
  char *key;
  struct ConfigObj *value;
};

struct objKv *config_kv_init(struct objKv *c);

struct ConfigMap {
  struct ConfigObj obj;
  struct objKv *kvs;
  size_t len;
  size_t cap;
};

struct ConfigMap *config_map_init(struct ConfigMap *c);

struct ConfigArray {
  struct ConfigObj obj;
  struct ConfigObj **data;
  size_t len;
  size_t cap;
};

struct ConfigArray *config_array_init(struct ConfigArray *c);

char *config_map_get_string(struct ConfigObj *c, const char *key);

#define CONFIG_TRAIT(obj_ptr, oper, args...) \
  ((((struct ConfigObj *) (obj_ptr))->trait->oper)(obj_ptr, ##args))

#define CONFIG_SAME_TYPE(obj_ptr, _type) \
  (((struct ConfigObj *) (obj_ptr))->trait->type(obj_ptr) == _type)

#define CONFIG_REQUIRE(obj_ptr, type) \
  MUST(CONFIG_SAME_TYPE(obj_ptr, type), "type not meet require")

#endif
