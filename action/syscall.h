#ifndef SYSCALL_H
#define SYSCALL_H

#include "config.h"

int process_syscall(void *file_trait, struct Operation *op);

int syscall_sys_write(void *file_trait, struct Operation *op);

#endif
