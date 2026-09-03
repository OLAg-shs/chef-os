#ifndef CHEF_KPRINTF_H
#define CHEF_KPRINTF_H

#include <stdarg.h>
#include "types.h"

void kprintf(const char *fmt, ...);
void kvprintf(const char *fmt, va_list args);

#endif
