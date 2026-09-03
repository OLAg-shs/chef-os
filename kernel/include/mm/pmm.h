#ifndef CHEF_PMM_H
#define CHEF_PMM_H

#include "types.h"
#include "limine.h"

#define PAGE_SIZE 4096

void pmm_init(struct limine_memmap_response *memmap_response, uint64_t hhdm_offset);
void *pmm_alloc_page(void);
void *pmm_alloc_pages(size_t count);
void pmm_free_page(void *ptr);
void pmm_free_pages(void *ptr, size_t count);
uint64_t pmm_get_free_memory(void);
uint64_t pmm_get_total_memory(void);

#endif
