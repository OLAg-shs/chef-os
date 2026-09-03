#include "mm/vmm.h"
#include "mm/pmm.h"
#include "string.h"

static uint64_t s_hhdm = 0;
static uint64_t *s_kernel_pml4 = NULL;

void vmm_init(uint64_t hhdm_offset) {
    s_hhdm = hhdm_offset;
    s_kernel_pml4 = (uint64_t *)((uint64_t)pmm_alloc_page() + s_hhdm);
    memset(s_kernel_pml4, 0, PAGE_SIZE);

    // Map 4GB in 2MB large pages via HHDM for maximum speed and efficiency
    uint64_t pml4_phys = (uint64_t)pmm_alloc_page();
    uint64_t *pdpt = (uint64_t *)(pml4_phys + s_hhdm);
    memset(pdpt, 0, PAGE_SIZE);

    uint64_t pml4_idx = (s_hhdm >> 39) & 0x1FF;
    s_kernel_pml4[pml4_idx] = pml4_phys | PTE_PRESENT | PTE_WRITABLE;

    for (uint64_t i = 0; i < 4; i++) {
        uint64_t pd_phys = (uint64_t)pmm_alloc_page();
        uint64_t *pd = (uint64_t *)(pd_phys + s_hhdm);
        memset(pd, 0, PAGE_SIZE);

        pdpt[i] = pd_phys | PTE_PRESENT | PTE_WRITABLE;

        for (uint64_t j = 0; j < 512; j++) {
            uint64_t phys_addr = (i * 512 + j) * (2 * 1024 * 1024);
            pd[j] = phys_addr | PTE_PRESENT | PTE_WRITABLE | (1ULL << 7); // 2MB Page Size bit
        }
    }
}

void vmm_map_page(uint64_t *pml4, virt_addr_t virt, phys_addr_t phys, uint64_t flags) {
    uint64_t pml4_idx = (virt >> 39) & 0x1FF;
    uint64_t pdpt_idx = (virt >> 30) & 0x1FF;
    uint64_t pd_idx   = (virt >> 21) & 0x1FF;
    uint64_t pt_idx   = (virt >> 12) & 0x1FF;

    if (!(pml4[pml4_idx] & PTE_PRESENT)) {
        uint64_t pdpt_phys = (uint64_t)pmm_alloc_page();
        memset((void *)(pdpt_phys + s_hhdm), 0, PAGE_SIZE);
        pml4[pml4_idx] = pdpt_phys | flags | PTE_PRESENT | PTE_WRITABLE;
    }
    uint64_t *pdpt = (uint64_t *)((pml4[pml4_idx] & ~0xFFFULL) + s_hhdm);

    if (!(pdpt[pdpt_idx] & PTE_PRESENT)) {
        uint64_t pd_phys = (uint64_t)pmm_alloc_page();
        memset((void *)(pd_phys + s_hhdm), 0, PAGE_SIZE);
        pdpt[pdpt_idx] = pd_phys | flags | PTE_PRESENT | PTE_WRITABLE;
    }
    uint64_t *pd = (uint64_t *)((pdpt[pdpt_idx] & ~0xFFFULL) + s_hhdm);

    if (!(pd[pd_idx] & PTE_PRESENT)) {
        uint64_t pt_phys = (uint64_t)pmm_alloc_page();
        memset((void *)(pt_phys + s_hhdm), 0, PAGE_SIZE);
        pd[pd_idx] = pt_phys | flags | PTE_PRESENT | PTE_WRITABLE;
    }
    uint64_t *pt = (uint64_t *)((pd[pd_idx] & ~0xFFFULL) + s_hhdm);

    pt[pt_idx] = phys | flags | PTE_PRESENT;
    __asm__ volatile("invlpg (%0)" : : "r"(virt) : "memory");
}

void vmm_switch_pml4(uint64_t *pml4) {
    uint64_t phys = (uint64_t)pml4 - s_hhdm;
    __asm__ volatile("mov %0, %%cr3" : : "r"(phys) : "memory");
}
