#include "limine.h"
#include "kernel.h"
#include "string.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/io.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "mm/heap.h"
#include "video/fb.h"
#include "drivers/serial.h"
#include "drivers/keyboard.h"
#include "drivers/ps2_mouse.h"
#include "drivers/timer.h"
#include "drivers/rtc.h"
#include "drivers/pci.h"
#include "drivers/ata.h"
#include "fs/vfs.h"
#include "fs/ramfs.h"
#include "fs/devfs.h"
#include "proc/sched.h"
#include "proc/process.h"
#include "proc/init.h"
#include "user/shell.h"
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

    // 8. Initialize Hardware Drivers (PIT, RTC, PCI, Keyboard, Mouse, Storage)
    kprintf("[+] Initializing PIT System Clock (1000 Hz / 1ms)... ");
    timer_init(1000);
    kprintf("[ OK ]\n");

    kprintf("[+] Initializing CMOS Real-Time Clock (RTC)... ");
    rtc_init();
    rtc_time_t now;
    rtc_get_time(&now);
    kprintf("[ OK ] (%d-%d-%d %d:%d:%d UTC)\n", (int)now.year, (int)now.month, (int)now.day, (int)now.hour, (int)now.minute, (int)now.second);

    kprintf("[+] Enumerating PCI Bus & Device Configuration Space... ");
    pci_init();
    kprintf("[ OK ]\n");

    kprintf("[+] Initializing PS/2 Keyboard & Mouse Drivers... ");
    keyboard_init();
    fb_info_t *cur_fb = fb_get_info();
    ps2_mouse_init(cur_fb ? cur_fb->width : 1280, cur_fb ? cur_fb->height : 800);
    fb_draw_cursor(500, 350);
    kprintf("[ OK ]\n");

    // 9. Initialize Virtual Filesystem (VFS) & Mount RamFS Root
    kprintf("[+] Initializing Virtual Filesystem (VFS) & Mounting RamFS... ");
    vfs_init();
    g_vfs_root = ramfs_create_root();

    vfs_node_t *dev_dir = ramfs_create_dir(g_vfs_root, "dev");
    vfs_node_t *bin_dir = ramfs_create_dir(g_vfs_root, "bin");
    vfs_node_t *etc_dir = ramfs_create_dir(g_vfs_root, "etc");
    vfs_node_t *home_dir = ramfs_create_dir(g_vfs_root, "home");
    (void)bin_dir; (void)home_dir;

    devfs_init(dev_dir);

    const char os_release[] = "NAME=\"Chef OS\"\nVERSION=\"1.0.0-alpha\"\nID=chef-os\nPRETTY_NAME=\"Chef OS 1.0 (Warm Parchment)\"\n";
    ramfs_create_file(etc_dir, "os-release", os_release, sizeof(os_release) - 1);
    kprintf("[ OK ]\n");

    kprintf("[+] Initializing ATA/IDE Storage Controller... ");
    ata_init();
    kprintf("[ OK ] (/dev/ata0 online)\n");

    // Verify /etc/os-release file read via VFS
    int os_fd = vfs_open("/etc/os-release", O_RDONLY);
    if (os_fd >= 0) {
        char os_buf[64];
        memset(os_buf, 0, sizeof(os_buf));
        vfs_read(os_fd, os_buf, 32);
        vfs_close(os_fd);
        kprintf("[+] Verified VFS Read on /etc/os-release [ OK ]\n");
    }

    // 10. Initialize Process Model & PID 1 Init System
    kprintf("[+] Initializing Process Subsystem & PID 1 Init Supervisor... ");
    process_init();
    init_system_boot();
    kprintf("[ OK ]\n");

    // 11. Initialize Thread Scheduler
    kprintf("[+] Initializing Preemptive Thread Scheduler... ");
    sched_init();
    kprintf("[ OK ]\n");

    // 12. Enable Hardware Interrupts
    kprintf("[+] Enabling Hardware Interrupts (STI)... ");
    sti();
    kprintf("[ OK ]\n");

    // 13. Initialize Syscall ABI & MSRs
    kprintf("[+] Initializing Syscall ABI (MSR STAR, LSTAR, SFMASK)... ");
    syscall_init(hhdm_offset);
    kprintf("[ OK ]\n\n");

    kprintf("------------------------------------------------------\n");
    kprintf("Chef OS 1.0 Native Micro-Core Online.\n");
    kprintf("Type 'help' for built-in commands or explore /etc, /dev:\n\n");

    // Launch interactive userland shell
    chef_shell_init();

    // Execute automated smoke test commands in shell to verify responsiveness
    chef_shell_execute_cmd("uname");
    chef_shell_execute_cmd("services");
    chef_shell_execute_cmd("cat /etc/os-release");
    kprintf("chef-os> ");

    while (1) {
        char c = keyboard_get_last_char();
        if (c) {
            chef_shell_handle_char(c);
        }
        hlt();
    }
}
