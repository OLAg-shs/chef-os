#ifndef CHEF_VMM_H
#define CHEF_VMM_H

#include "types.h"

#define PTE_PRESENT  (1ULL << 0)
#define PTE_WRITABLE (1ULL << 1)
#define PTE_USER     (1ULL << 2)

void vmm_init(uint64_t hhdm_offset);
uint64_t *vmm_get_active_pml4(void);
void vmm_map_page(uint64_t *pml4, virt_addr_t virt, phys_addr_t phys, uint64_t flags);

#endif
