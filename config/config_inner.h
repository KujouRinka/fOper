#ifndef CONFIG_INNER_H
#define CONFIG_INNER_H

#include <stddef.h>
#include <stdint.h>
#include "common.h"

enum ConfigType {
  CONFIG_TYPE_NULL = 0,
  CONFIG_TYPE_INT,
  CONFIG_TYPE_FLOAT,
  CONFIG_TYPE_STRING,
  CONFIG_TYPE_BOOL,
  CONFIG_TYPE_MAP,
  CONFIG_TYPE_ARRAY,
};

struct ConfigObj;
struct ConfigInt;
struct ConfigFloat;
struct ConfigString;
struct ConfigBool;
struct ConfigMap;
struct ConfigArray;

struct objKv;

struct ConfigObj *parse_config_v2(const char *filename);

struct ConfigObjTrait {
  enum ConfigType (*type)(void *self);
  int (*get_int)(void *self);
  float (*get_float)(void *self);
  char *(*get_string)(void *self);
  int (*get_bool)(void *self);
  struct ConfigObj *(*get_map)(void *self, const char *key);
  struct objKv *(*get_map_by_index)(void *self, int index);
  int (*get_map_size)(void *self);
  struct ConfigObj *(*get_array)(void *self, int index);
  int (*get_array_size)(void *self);

  struct ConfigObj (*parse)(void *self);
  int (*free)(void *self);
};

extern struct ConfigObjTrait defaultConfigTrait;

static enum ConfigType config_obj_type_default(void *self);
static int config_obj_get_int_default(void *self);
static float config_obj_get_float_default(void *self);
static char *config_obj_get_string_default(void *self);
static int config_obj_get_bool_default(void *self);
static struct ConfigObj *config_obj_get_map_default(void *self, const char *key);
static struct objKv *config_obj_get_map_by_index_default(void *self, int index);
static int config_obj_get_map_size_default(void *self);
static struct ConfigObj *config_obj_get_array_default(void *self, int index);
static int config_obj_get_array_size_default(void *self);
static int config_obj_free_default(void *self);

struct ConfigObj {
  struct ConfigObjTrait *trait;
  enum ConfigType type;
};

struct ConfigInt {
  struct ConfigObj obj;
  int value;
};

struct ConfigFloat {
  struct ConfigObj obj;
  float value;
};

struct ConfigString {
  struct ConfigObj obj;
  uint8_t *data;
  size_t len;
};

struct ConfigBool {
  struct ConfigObj obj;
  int value;
};

struct objKv {
  char *key;
  struct ConfigObj *value;
};

struct ConfigMap {
  struct ConfigObj obj;
  struct objKv *kvs;
  size_t len;
};

struct ConfigArray {
  struct ConfigObj obj;
  struct ConfigObj *data;
  size_t len;
};

#define CONFIG_TRAIT(obj_ptr, oper, args...) \
  ((((struct ConfigObj *)(obj_ptr))->trait->oper)(obj_ptr, ##args))

#define CONFIG_SAME_TYPE(obj_ptr, _type) \
  (((struct ConfigObj *)(obj_ptr))->trait->type(obj_ptr) == _type)

#define CONFIG_REQUIRE(obj_ptr, type) \
  MUST(CONFIG_SAME_TYPE(obj_ptr, type), "type not meet require")

#endif
