#include "sys/syscall.h"
#include "arch/gdt.h"
#include "arch/io.h"
#include "video/fb.h"
#include "drivers/serial.h"
#include "drivers/keyboard.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "kprintf.h"
#include "string.h"

extern void syscall_entry_stub(void);
extern uint8_t user_payload_code[];
extern uint8_t user_payload_code_end[];
extern uint64_t g_kernel_syscall_stack;

#define MSR_EFER   0xC0000080
#define MSR_STAR   0xC0000081
#define MSR_LSTAR  0xC0000082
#define MSR_SFMASK 0xC0000084

static uint64_t s_hhdm = 0xFFFF800000000000ULL;

static inline void wrmsr(uint32_t msr, uint64_t val) {
    uint32_t low = val & 0xFFFFFFFF;
    uint32_t high = val >> 32;
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

void syscall_init(uint64_t hhdm_offset) {
    s_hhdm = hhdm_offset;
    // 1. Allocate 16KB dedicated kernel stack for syscall execution
    void *stack_phys = pmm_alloc_pages(4);
    g_kernel_syscall_stack = (uint64_t)stack_phys + s_hhdm + (4 * PAGE_SIZE) - 16;
    tss_set_kernel_stack(g_kernel_syscall_stack);

    // 2. Enable SCE (System Call Enable) bit in IA32_EFER
    uint64_t efer = rdmsr(MSR_EFER);
    efer |= 1; // Bit 0 = SCE
    wrmsr(MSR_EFER, efer);

    // 3. Configure STAR MSR
    // Bits [47:32] = Kernel CS (0x08) & SS (0x10)
    // Bits [63:48] = User Base (0x10) -> User SS = 0x18 | 3, User CS = 0x20 | 3
    uint64_t star = ((uint64_t)0x0010 << 48) | ((uint64_t)0x0008 << 32);
    wrmsr(MSR_STAR, star);

    // 4. Configure LSTAR MSR with address of entry stub
    wrmsr(MSR_LSTAR, (uint64_t)syscall_entry_stub);

    // 5. Configure SFMASK MSR (mask IF, TF, DF, NT)
    wrmsr(MSR_SFMASK, 0x200 | 0x100 | 0x400);
}

int64_t syscall_dispatch(uint64_t num, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6) {
    (void)a4; (void)a5; (void)a6;
    switch (num) {
        case SYS_WRITE: {
            int fd = (int)a1;
            const char *buf = (const char *)a2;
            size_t count = (size_t)a3;
            if (fd == 1 || fd == 2) {
                for (size_t i = 0; i < count; i++) {
                    serial_putc(buf[i]);
                    fb_putc(buf[i]);
                }
                fb_swap();
                return (int64_t)count;
            }
            return -1;
        }
        case SYS_READ: {
            int fd = (int)a1;
            char *buf = (char *)a2;
            if (fd == 0 && a3 > 0) {
                char c = keyboard_get_last_char();
                if (c) {
                    buf[0] = c;
                    return 1;
                }
                return 0;
            }
            return -1;
        }
        case SYS_GETPID:
            return 1;
        case SYS_YIELD:
            return 0;
        case SYS_DRAW_RECT: {
            uint32_t x = (uint32_t)a1;
            uint32_t y = (uint32_t)a2;
            uint32_t w = (uint32_t)a3;
            uint32_t h = (uint32_t)a4;
            uint32_t color = (uint32_t)a5;
            fb_draw_rect(x, y, w, h, color);
            return 0;
        }
        case SYS_SWAP_BUFFERS:
            fb_swap();
            return 0;
        default:
            kprintf("[SYSCALL] Unknown syscall #%d\n", (int)num);
            return -1;
    }
}

void user_mode_enter_test(void) {
    uint64_t *active_pml4 = vmm_get_active_pml4();

    // 1. Allocate and map user code page at 0x400000
    void *user_code_phys = pmm_alloc_page();
    uint64_t user_entry_virt = 0x400000;
    vmm_map_page(active_pml4, user_entry_virt, (uint64_t)user_code_phys, PTE_PRESENT | PTE_WRITABLE | PTE_USER);

    // Copy user payload machine code to the page
    size_t payload_len = (size_t)(user_payload_code_end - user_payload_code);
    memcpy((void *)((uint64_t)user_code_phys + s_hhdm), user_payload_code, payload_len);

    // 2. Allocate and map user stack page at 0x7FFFFFFFE000
    void *user_stack_phys = pmm_alloc_page();
    uint64_t user_stack_base = 0x7FFFFFFFE000;
    vmm_map_page(active_pml4, user_stack_base, (uint64_t)user_stack_phys, PTE_PRESENT | PTE_WRITABLE | PTE_USER);
    uint64_t user_rsp = user_stack_base + PAGE_SIZE - 16;

    // 3. Jump to Ring 3 using iretq
    uint64_t user_ss = 0x18 | 3; // User Data Selector (0x1B)
    uint64_t user_cs = 0x20 | 3; // User Code Selector (0x23)
    uint64_t rflags = 0x202;     // Interrupts enabled

    __asm__ volatile (
        "mov %0, %%ds\n"
        "mov %0, %%es\n"
        "mov %0, %%fs\n"
        "mov %0, %%gs\n"
        "push %0\n"          // SS
        "push %1\n"          // RSP
        "push %2\n"          // RFLAGS
        "push %3\n"          // CS
        "push %4\n"          // RIP
        "iretq\n"
        :
        : "r"(user_ss), "r"(user_rsp), "r"(rflags), "r"(user_cs), "r"(user_entry_virt)
        : "memory"
    );
}
