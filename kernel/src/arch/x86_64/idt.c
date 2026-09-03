#include "arch/idt.h"
#include "arch/pic.h"
#include "kprintf.h"
#include "string.h"

extern void idt_flush(uint64_t idt_ptr);

// External ISR prototypes from isr_asm.S
#define DECL_ISR(n) extern void isr##n(void);
DECL_ISR(0) DECL_ISR(1) DECL_ISR(2) DECL_ISR(3) DECL_ISR(4) DECL_ISR(5) DECL_ISR(6) DECL_ISR(7)
DECL_ISR(8) DECL_ISR(9) DECL_ISR(10) DECL_ISR(11) DECL_ISR(12) DECL_ISR(13) DECL_ISR(14) DECL_ISR(15)
DECL_ISR(16) DECL_ISR(17) DECL_ISR(18) DECL_ISR(19) DECL_ISR(20) DECL_ISR(21) DECL_ISR(22) DECL_ISR(23)
DECL_ISR(24) DECL_ISR(25) DECL_ISR(26) DECL_ISR(27) DECL_ISR(28) DECL_ISR(29) DECL_ISR(30) DECL_ISR(31)

#define DECL_IRQ(n) extern void irq##n(void);
DECL_IRQ(0) DECL_IRQ(1) DECL_IRQ(2) DECL_IRQ(3) DECL_IRQ(4) DECL_IRQ(5) DECL_IRQ(6) DECL_IRQ(7)
DECL_IRQ(8) DECL_IRQ(9) DECL_IRQ(10) DECL_IRQ(11) DECL_IRQ(12) DECL_IRQ(13) DECL_IRQ(14) DECL_IRQ(15)

static idt_entry_t s_idt[256];
static idt_ptr_t   s_idt_ptr;
static isr_handler_t s_handlers[256];

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    s_idt[num].offset_low      = base & 0xFFFF;
    s_idt[num].selector        = sel;
    s_idt[num].ist             = 0;
    s_idt[num].type_attributes = flags;
    s_idt[num].offset_mid      = (base >> 16) & 0xFFFF;
    s_idt[num].offset_high     = (base >> 32) & 0xFFFFFFFF;
    s_idt[num].reserved        = 0;
}

void register_interrupt_handler(uint8_t n, isr_handler_t handler) {
    s_handlers[n] = handler;
}

void isr_handler(registers_t *regs) {
    if (s_handlers[regs->int_no] != NULL) {
        s_handlers[regs->int_no](regs);
    } else if (regs->int_no < 32) {
        kprintf("\n[KERNEL PANIC] Unhandled Exception: #%d (Error: 0x%x) at RIP: 0x%p\n",
                regs->int_no, regs->err_code, regs->rip);
        while (1) __asm__ volatile ("hlt");
    }

    if (regs->int_no >= 32 && regs->int_no < 48) {
        pic_send_eoi(regs->int_no - 32);
    }
}

void idt_init(void) {
    s_idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    s_idt_ptr.base  = (uint64_t)&s_idt;
    memset(&s_idt, 0, sizeof(idt_entry_t) * 256);
    memset(&s_handlers, 0, sizeof(isr_handler_t) * 256);

    // Remap PIC IRQs 0..15 -> 32..47
    pic_remap(32, 40);

    // Exception gates
    #define SET_ISR(n) idt_set_gate(n, (uint64_t)isr##n, 0x08, 0x8E);
    SET_ISR(0); SET_ISR(1); SET_ISR(2); SET_ISR(3); SET_ISR(4); SET_ISR(5); SET_ISR(6); SET_ISR(7);
    SET_ISR(8); SET_ISR(9); SET_ISR(10); SET_ISR(11); SET_ISR(12); SET_ISR(13); SET_ISR(14); SET_ISR(15);
    SET_ISR(16); SET_ISR(17); SET_ISR(18); SET_ISR(19); SET_ISR(20); SET_ISR(21); SET_ISR(22); SET_ISR(23);
    SET_ISR(24); SET_ISR(25); SET_ISR(26); SET_ISR(27); SET_ISR(28); SET_ISR(29); SET_ISR(30); SET_ISR(31);

    // IRQ gates
    #define SET_IRQ(n, intno) idt_set_gate(intno, (uint64_t)irq##n, 0x08, 0x8E);
    SET_IRQ(0, 32); SET_IRQ(1, 33); SET_IRQ(2, 34); SET_IRQ(3, 35);
    SET_IRQ(4, 36); SET_IRQ(5, 37); SET_IRQ(6, 38); SET_IRQ(7, 39);
    SET_IRQ(8, 40); SET_IRQ(9, 41); SET_IRQ(10, 42); SET_IRQ(11, 43);
    SET_IRQ(12, 44); SET_IRQ(13, 45); SET_IRQ(14, 46); SET_IRQ(15, 47);

    idt_flush((uint64_t)&s_idt_ptr);
}
