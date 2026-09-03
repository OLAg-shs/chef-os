#ifndef CHEF_KERNEL_H
#define CHEF_KERNEL_H

#include "types.h"
#include "kprintf.h"

#define CHEF_OS_VERSION "1.0.0-alpha"
#define CHEF_OS_CODENAME "Warm Parchment"

void panic(const char *msg);
void halt(void);

#endif
