#include "config/config.h"

#include <stdio.h>

#include "common.h"

int main() {
  LOG("\033[1;32mTest Config Begin\033[0m\n");

  struct ConfigObj *config = parse_config_v2("./test/hy.yaml");
  EXPECT_NE(config, NULL, "config file is null");
  CONFIG_REQUIRE(config, CONFIG_TYPE_MAP);

  struct ConfigObj *cs;
  cs = CONFIG_TRAIT(config, as_map, "server");
  CONFIG_REQUIRE(cs, CONFIG_TYPE_BYTE);
  struct ConfigByte *server = CONFIG_TRAIT(cs, as_byte);
  EXPECT_EQ(memcmp(server->data, "114.51.41.91", server->len), 0, "server is not equal");

  cs = CONFIG_TRAIT(config, as_map, "auth");
  CONFIG_REQUIRE(cs, CONFIG_TYPE_BYTE);
  EXPECT_EQ(strcmp(CONFIG_TRAIT(cs, as_string), "haaaa114514"), 0, "auth is not equal");

  cs = CONFIG_TRAIT(config, as_map, "tls");
  CONFIG_REQUIRE(cs, CONFIG_TYPE_MAP);
  struct ConfigByte *sni = (struct ConfigByte *) CONFIG_TRAIT(cs, as_map, "sni");
  CONFIG_REQUIRE(sni, CONFIG_TYPE_BYTE);
  EXPECT_EQ(strcmp(CONFIG_TRAIT(sni, as_string), "0x7fffffff.eu.org"), 0, "sni is not equal");

  struct ConfigByte *insecure = (struct ConfigByte *) CONFIG_TRAIT(cs, as_map, "insecure");
  CONFIG_REQUIRE(insecure, CONFIG_TYPE_BYTE);
  EXPECT_EQ(memcmp(insecure->data, "false", insecure->len), 0, "insecure is not equal");

  struct ConfigObj *bandwidth = CONFIG_TRAIT(config, as_map, "bandwidth");
  CONFIG_REQUIRE(bandwidth, CONFIG_TYPE_MAP);
  struct ConfigByte *up = (struct ConfigByte *) CONFIG_TRAIT(bandwidth, as_map, "up");
  CONFIG_REQUIRE(up, CONFIG_TYPE_BYTE);
  EXPECT_EQ(memcmp(up->data, "80 mbps", up->len), 0, "up is not equal");

  struct ConfigByte *down = (struct ConfigByte *) CONFIG_TRAIT(bandwidth, as_map, "down");
  CONFIG_REQUIRE(down, CONFIG_TYPE_BYTE);
  EXPECT_EQ(memcmp(down->data, "170 mbps", down->len), 0, "down is not equal");

  struct ConfigObj *http = CONFIG_TRAIT(config, as_map, "http");
  CONFIG_REQUIRE(http, CONFIG_TYPE_MAP);
  struct ConfigByte *listen = (struct ConfigByte *) CONFIG_TRAIT(http, as_map, "listen");
  CONFIG_REQUIRE(listen, CONFIG_TYPE_BYTE);
  EXPECT_EQ(memcmp(listen->data, "127.0.0.1:8889", listen->len), 0, "listen is not equal");

  LOG("\033[1;32mTest Config Passed!\033[0m\n");
  return 0;
}
