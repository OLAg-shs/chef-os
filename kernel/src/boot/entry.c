#include "limine.h"
#include "kernel.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/io.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "mm/heap.h"
#include "video/fb.h"
#include "drivers/serial.h"
#include "drivers/keyboard.h"
#include "proc/sched.h"
#include "sys/syscall.h"

// Limine Requests
__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request fb_req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_memmap_request memmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

void panic(const char *msg) {
    cli();
    kprintf("\n======================================================\n");
    kprintf("                CHEF OS KERNEL PANIC                  \n");
    kprintf("======================================================\n");
    kprintf("Reason: %s\n", msg);
    kprintf("System halted. Please power off or restart.\n");
    halt();
}

void halt(void) {
    while (1) {
        cli();
        hlt();
    }
}

void kmain(void) {
    // 1. Initialize Serial Port for COM1 debug logging
    serial_init();
    serial_puts("\n[CHEF OS] Kernel Entry Initialized.\n");

    // 2. Initialize Framebuffer & Desktop Shell
    if (fb_req.response && fb_req.response->framebuffer_count > 0) {
        fb_init(fb_req.response->framebuffers[0]);
        fb_draw_desktop_shell();
    }

    kprintf("======================================================\n");
    kprintf("  Chef OS 1.0 (Codename: %s)\n", CHEF_OS_CODENAME);
    kprintf("  Architecture: x86_64 Higher-Half Native Micro-Core\n");
    kprintf("======================================================\n\n");

    // 3. Initialize GDT & TSS
    kprintf("[+] Initializing Global Descriptor Table (GDT) & TSS... ");
    gdt_init();
    kprintf("[ OK ]\n");

    // 4. Initialize IDT & Exceptions
    kprintf("[+] Initializing Interrupt Descriptor Table (IDT)... ");
    idt_init();
    kprintf("[ OK ]\n");

    // 5. Initialize Physical Memory Manager (PMM)
    kprintf("[+] Initializing Physical Memory Manager (Bitmap Allocator)... ");
    uint64_t hhdm_offset = hhdm_req.response ? hhdm_req.response->offset : 0;
    pmm_init(memmap_req.response, hhdm_offset);
    uint64_t total_mb = pmm_get_total_memory() / (1024 * 1024);
    uint64_t free_mb = pmm_get_free_memory() / (1024 * 1024);
    kprintf("[ OK ] (%d MB Total, %d MB Free)\n", (int)total_mb, (int)free_mb);

    // 6. Initialize Virtual Memory Manager (VMM)
    kprintf("[+] Initializing 4-Level Paging (VMM & HHDM Mappings)... ");
    vmm_init(hhdm_offset);
    kprintf("[ OK ]\n");

    // 7. Initialize Kernel Heap
    kprintf("[+] Initializing Kernel Heap Allocator (kmalloc/kfree)... ");
    heap_init(hhdm_offset);
    kprintf("[ OK ]\n");

    // 8. Initialize PS/2 Keyboard Driver
    kprintf("[+] Initializing PS/2 Keyboard Driver... ");
    keyboard_init();
    kprintf("[ OK ]\n");

    // 9. Initialize Thread Scheduler
    kprintf("[+] Initializing Preemptive Thread Scheduler... ");
    sched_init();
    kprintf("[ OK ]\n");

    // 10. Enable Hardware Interrupts
    kprintf("[+] Enabling Hardware Interrupts (STI)... ");
    sti();
    kprintf("[ OK ]\n");

    // 11. Initialize Syscall ABI & MSRs
    kprintf("[+] Initializing Syscall ABI (MSR STAR, LSTAR, SFMASK)... ");
    syscall_init(hhdm_offset);
    kprintf("[ OK ]\n\n");

    kprintf("------------------------------------------------------\n");
    kprintf("Chef OS Kernel Foundation & Syscall Layer Online.\n");
    kprintf("Testing Ring 3 User Mode transition & native syscall:\n");

    // 12. Switch to Ring 3 User Mode & Execute Syscall Test
    user_mode_enter_test();

    while (1) {
        hlt();
    }
}
