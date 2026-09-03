#include "arch/gdt.h"
#include "string.h"

extern void gdt_flush(uint64_t gdt_ptr);
extern void tss_flush(void);

static gdt_entry_t s_gdt[7];
static gdt_ptr_t   s_gdt_ptr;
static tss_t       s_tss;

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    s_gdt[num].base_low    = (base & 0xFFFF);
    s_gdt[num].base_middle = (base >> 16) & 0xFF;
    s_gdt[num].base_high   = (base >> 24) & 0xFF;
    s_gdt[num].limit_low   = (limit & 0xFFFF);
    s_gdt[num].granularity = (limit >> 16) & 0x0F;
    s_gdt[num].granularity |= gran & 0xF0;
    s_gdt[num].access      = access;
}

void gdt_init(void) {
    s_gdt_ptr.limit = (sizeof(gdt_entry_t) * 7) - 1;
    s_gdt_ptr.base  = (uint64_t)&s_gdt;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null descriptor
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xA0);    // 64-bit Kernel Code (0x08)
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xC0);    // 64-bit Kernel Data (0x10)
    gdt_set_gate(3, 0, 0xFFFFF, 0xFA, 0xA0);    // 64-bit User Code   (0x18)
    gdt_set_gate(4, 0, 0xFFFFF, 0xF2, 0xC0);    // 64-bit User Data   (0x20)

    // TSS descriptor (takes 2 entries in 64-bit mode)
    memset(&s_tss, 0, sizeof(tss_t));
    s_tss.iomap_base = sizeof(tss_t);

    uint64_t tss_base = (uint64_t)&s_tss;
    uint32_t tss_limit = sizeof(tss_t) - 1;

    gdt_set_gate(5, tss_base & 0xFFFFFFFF, tss_limit, 0x89, 0x00);
    // Upper 32 bits of TSS base
    uint32_t *tss_high = (uint32_t *)&s_gdt[6];
    *tss_high = (uint32_t)(tss_base >> 32);

    gdt_flush((uint64_t)&s_gdt_ptr);
    tss_flush();
}
