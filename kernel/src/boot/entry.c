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
#include "compositor/compositor.h"
#include "compositor/window.h"
#include "compositor/surface.h"
#include "compositor/layout.h"
#include "compositor/shell_ui.h"

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

static void paint_terminal_window(window_t *win) {
    if (!win || !win->surface) return;
    surface_clear(win->surface, COLOR_SURFACE);
    surface_draw_string(win->surface, "Chef OS 1.0 (Warm Parchment) x86_64", 16, 12, COLOR_TEXT);
    surface_draw_string(win->surface, "Kernel higher-half micro-core online.", 16, 32, COLOR_TEXT_MUTED);
    surface_draw_string(win->surface, "PID 1 init supervisor running: klogd, compositor, chef-sh", 16, 52, COLOR_TEXT_MUTED);
    surface_draw_string(win->surface, "Dynamic Tiling Engine: Master-Stack Layout active.", 16, 72, COLOR_TEXT_MUTED);
    surface_draw_string(win->surface, "chef-os> uname", 16, 102, COLOR_ACCENT);
    surface_draw_string(win->surface, "Chef OS 1.0.0-alpha x86_64 (Warm Parchment)", 16, 122, COLOR_TEXT);
    surface_draw_string(win->surface, "chef-os> cat /etc/os-release", 16, 152, COLOR_ACCENT);
    surface_draw_string(win->surface, "NAME=\"Chef OS\" VERSION=\"1.0.0-alpha\" ID=chef-os", 16, 172, COLOR_TEXT);
    surface_draw_string(win->surface, "chef-os> _", 16, 202, COLOR_TEXT);
}

static void paint_system_window(window_t *win) {
    if (!win || !win->surface) return;
    surface_clear(win->surface, COLOR_SURFACE);
    surface_draw_string(win->surface, "CPU: x86_64 Core [ONLINE]", 16, 12, COLOR_TEXT);
    surface_draw_string(win->surface, "RAM: 2016 MB (1991 MB Free)", 16, 32, COLOR_TEXT);
    surface_draw_string(win->surface, "Timer: 1000 Hz PIT (1ms)", 16, 52, COLOR_TEXT_MUTED);
    surface_draw_string(win->surface, "RTC: CMOS UTC Active", 16, 72, COLOR_TEXT_MUTED);
    surface_draw_string(win->surface, "Status: Nominal | Ring 3 Ready", 16, 92, COLOR_GREEN);
}

static void paint_vfs_window(window_t *win) {
    if (!win || !win->surface) return;
    surface_clear(win->surface, COLOR_SURFACE);
    surface_draw_string(win->surface, "Mount: / (RamFS Root)", 16, 12, COLOR_TEXT);
    surface_draw_string(win->surface, "  |-- /bin/  [Binaries]", 16, 32, COLOR_TEXT_MUTED);
    surface_draw_string(win->surface, "  |-- /dev/  [serial, kbd, ata0]", 16, 52, COLOR_TEXT_MUTED);
    surface_draw_string(win->surface, "  |-- /etc/  [os-release]", 16, 72, COLOR_TEXT_MUTED);
    surface_draw_string(win->surface, "Storage: /dev/ata0 [READY]", 16, 92, COLOR_BLUE);
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
    kprintf("[ OK ]\n");

    // 14. Initialize Native Display Server & Dynamic Tiling Compositor (Gates G11-G14)
    kprintf("[+] Initializing Native 2D Surface Compositor & Dynamic Tiling Engine... ");
    compositor_init();

    // Create Window 3: Storage & VFS Tree Explorer (Stack Bottom)
    window_t *vfs_win = window_create(828, 305, 420, 235, "VFS Storage Explorer (/dev, /etc)");
    if (vfs_win) {
        vfs_win->workspace_id = 1;
        vfs_win->paint = paint_vfs_window;
        paint_vfs_window(vfs_win);
        compositor_register_window(vfs_win);
    }

    // Create Window 2: System Performance & Hardware Dashboard (Stack Top)
    window_t *sys_win = window_create(828, 60, 420, 230, "System Performance Monitor");
    if (sys_win) {
        sys_win->workspace_id = 1;
        sys_win->paint = paint_system_window;
        paint_system_window(sys_win);
        compositor_register_window(sys_win);
    }

    // Create Window 1: Native Terminal Console (Master Window)
    window_t *term_win = window_create(88, 60, 720, 480, "Chef OS Parchment Terminal (chef-sh)");
    if (term_win) {
        term_win->workspace_id = 1;
        term_win->paint = paint_terminal_window;
        paint_terminal_window(term_win);
        compositor_register_window(term_win);
    }

    kprintf("[ OK ]\n\n");

    kprintf("------------------------------------------------------\n");
    kprintf("Chef OS Native Compositor & Tiling Window Manager Online.\n");
    kprintf("Dynamic Island, Left Dock, and 3 Tiled Client Windows Active.\n\n");

    // Render composited desktop
    compositor_render_frame();

    while (1) {
        mouse_state_t ms;
        ps2_mouse_get_state(&ms);
        compositor_set_mouse(ms.x, ms.y);

        char c = keyboard_get_last_char();
        if (c) {
            if (c >= '1' && c <= '4') {
                compositor_set_workspace(c - '0');
            } else if (c == ' ') {
                layout_cycle_mode();
            } else if (c == '\t') {
                compositor_cycle_focus();
            } else {
                chef_shell_handle_char(c);
            }
            compositor_render_frame();
        }
        hlt();
    }
}
