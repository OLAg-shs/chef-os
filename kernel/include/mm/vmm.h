#ifndef CHEF_VMM_H
#define CHEF_VMM_H

#include "types.h"

#define PTE_PRESENT  (1ULL << 0)
#define PTE_WRITABLE (1ULL << 1)
#define PTE_USER     (1ULL << 2)

typedef uint64_t page_table_t[512];

void vmm_init(uint64_t hhdm_offset);
void vmm_map_page(uint64_t *pml4, virt_addr_t virt, phys_addr_t phys, uint64_t flags);
void vmm_switch_pml4(uint64_t *pml4);

#endif
