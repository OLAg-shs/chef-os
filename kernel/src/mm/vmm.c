#include "mm/vmm.h"
#include "mm/pmm.h"
#include "string.h"

static uint64_t s_hhdm = 0;

void vmm_init(uint64_t hhdm_offset) {
    s_hhdm = hhdm_offset;
}

uint64_t *vmm_get_active_pml4(void) {
    uint64_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    return (uint64_t *)((cr3 & ~0xFFFULL) + s_hhdm);
}

void vmm_map_page(uint64_t *pml4, virt_addr_t virt, phys_addr_t phys, uint64_t flags) {
    uint64_t pml4_idx = (virt >> 39) & 0x1FF;
    uint64_t pdpt_idx = (virt >> 30) & 0x1FF;
    uint64_t pd_idx   = (virt >> 21) & 0x1FF;
    uint64_t pt_idx   = (virt >> 12) & 0x1FF;

    uint64_t table_flags = PTE_PRESENT | PTE_WRITABLE;
    if (flags & PTE_USER) {
        table_flags |= PTE_USER;
    }

    if (!(pml4[pml4_idx] & PTE_PRESENT)) {
        uint64_t pdpt_phys = (uint64_t)pmm_alloc_page();
        memset((void *)(pdpt_phys + s_hhdm), 0, PAGE_SIZE);
        pml4[pml4_idx] = pdpt_phys | table_flags;
    } else if (flags & PTE_USER) {
        pml4[pml4_idx] |= PTE_USER;
    }
    uint64_t *pdpt = (uint64_t *)((pml4[pml4_idx] & ~0xFFFULL) + s_hhdm);

    if (!(pdpt[pdpt_idx] & PTE_PRESENT)) {
        uint64_t pd_phys = (uint64_t)pmm_alloc_page();
        memset((void *)(pd_phys + s_hhdm), 0, PAGE_SIZE);
        pdpt[pdpt_idx] = pd_phys | table_flags;
    } else if (flags & PTE_USER) {
        pdpt[pdpt_idx] |= PTE_USER;
    }
    uint64_t *pd = (uint64_t *)((pdpt[pdpt_idx] & ~0xFFFULL) + s_hhdm);

    if (!(pd[pd_idx] & PTE_PRESENT)) {
        uint64_t pt_phys = (uint64_t)pmm_alloc_page();
        memset((void *)(pt_phys + s_hhdm), 0, PAGE_SIZE);
        pd[pd_idx] = pt_phys | table_flags;
    } else if (flags & PTE_USER) {
        pd[pd_idx] |= PTE_USER;
    }
    uint64_t *pt = (uint64_t *)((pd[pd_idx] & ~0xFFFULL) + s_hhdm);

    pt[pt_idx] = phys | flags | PTE_PRESENT;
    __asm__ volatile("invlpg (%0)" : : "r"(virt) : "memory");
}
