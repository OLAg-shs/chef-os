#ifndef CHEF_HEAP_H
#define CHEF_HEAP_H

#include "types.h"

void heap_init(uint64_t hhdm_offset);
void *kmalloc(size_t size);
void *kzalloc(size_t size);
void kfree(void *ptr);

#endif
