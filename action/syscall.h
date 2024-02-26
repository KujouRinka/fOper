#ifndef SYSCALL_H
#define SYSCALL_H

struct ConfigMap;

int process_syscall(void *file_trait, struct ConfigMap *op);
int syscall_sys_write(void *file_trait, struct ConfigMap *op);

#endif
