#ifndef STRPTIME_H
#define STRPTIME_H

#include <stdint.h>
#include <stdlib.h>

struct era_entry {
  uint32_t direction; /* Contains '+' or '-'.  */
  int32_t offset;
  int32_t start_date[3];
  int32_t stop_date[3];
  const char *era_name;
  const char *era_format;
  const wchar_t *era_wname;
  const wchar_t *era_wformat;
  int absolute_direction;
  /* absolute direction:
     +1 indicates that year number is higher in the future. (like A.D.)
     -1 indicates that year number is higher in the past. (like B.C.)  */
};

char *strptime(const char *buf, const char *format, struct tm *tm);

#endif
