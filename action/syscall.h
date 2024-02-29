#ifndef SYSCALL_H
#define SYSCALL_H

struct ConfigMap;

int process_syscall(void *file_trait, struct ConfigMap *op);

static int syscall_sys_write(void *file_trait, struct ConfigMap *op);
static int syscall_sys_writev(void *file_trait, struct ConfigMap *op);

#endif
