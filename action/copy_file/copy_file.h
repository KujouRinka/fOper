#ifndef ACTION_COPY_FILE_H
#define ACTION_COPY_FILE_H

struct ConfigObj;

typedef enum {
  Default,
  Mod,
  Copy,
} Action;

int copy_file(const char *dst, const char *src, struct ConfigObj *config);
int copy_file_to_dir(const char *dst, const char *src, struct ConfigObj *config);
int copy_dir(const char *dst, const char *src, struct ConfigObj *config);

static Action str2action(const char *action);

#endif
