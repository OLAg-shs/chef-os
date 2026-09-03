#include "user/shell.h"
#include "fs/vfs.h"
#include "fs/ramfs.h"
#include "proc/process.h"
#include "proc/init.h"
#include "drivers/rtc.h"
#include "drivers/timer.h"
#include "mm/pmm.h"
#include "video/fb.h"
#include "arch/io.h"
#include "string.h"
#include "kprintf.h"

static char s_input_buf[256];
static size_t s_input_len = 0;

void chef_shell_init(void) {
    s_input_len = 0;
    memset(s_input_buf, 0, sizeof(s_input_buf));
    kprintf("chef-os> ");
}

void chef_shell_execute_cmd(const char *cmdline) {
    while (*cmdline == ' ') cmdline++;
    if (*cmdline == '\0') return;

    if (strcmp(cmdline, "help") == 0) {
        kprintf("Available Built-in Commands:\n");
        kprintf("  help     - Display this command manual\n");
        kprintf("  uname    - Show OS version and system architecture\n");
        kprintf("  clear    - Clear the kernel console screen\n");
        kprintf("  ls       - List directory contents\n");
        kprintf("  cat      - Display contents of a text file\n");
        kprintf("  ps       - List running processes and memory footprint\n");
        kprintf("  services - List active system services managed by init\n");
        kprintf("  mem      - Display physical memory utilization\n");
        kprintf("  date     - Show current CMOS real-time clock timestamp\n");
        kprintf("  echo     - Print text arguments to console\n");
        kprintf("  reboot   - Reboot the machine\n");
        kprintf("  poweroff - Shut down system\n");
    } else if (strcmp(cmdline, "uname") == 0) {
        kprintf("Chef OS 1.0.0-alpha x86_64 Higher-Half (Codename: Warm Parchment)\n");
    } else if (strcmp(cmdline, "clear") == 0) {
        fb_draw_desktop_shell();
    } else if (strcmp(cmdline, "ls") == 0) {
        kprintf("  Directory of /\n");
        kprintf("  drwxr-xr-x   bin\n");
        kprintf("  drwxr-xr-x   dev\n");
        kprintf("  drwxr-xr-x   etc\n");
        kprintf("  drwxr-xr-x   home\n");
    } else if (strncmp(cmdline, "cat ", 4) == 0) {
        const char *path = cmdline + 4;
        int fd = vfs_open(path, O_RDONLY);
        if (fd >= 0) {
            char buf[512];
            memset(buf, 0, sizeof(buf));
            int64_t n = vfs_read(fd, buf, 511);
            vfs_close(fd);
            if (n > 0) {
                kprintf("%s\n", buf);
            }
        } else {
            kprintf("cat: %s: No such file or directory\n", path);
        }
    } else if (strcmp(cmdline, "ps") == 0) {
        process_list();
    } else if (strcmp(cmdline, "services") == 0) {
        init_list_services();
    } else if (strcmp(cmdline, "mem") == 0) {
        uint64_t total = pmm_get_total_memory() / (1024 * 1024);
        uint64_t free = pmm_get_free_memory() / (1024 * 1024);
        kprintf("Memory Utilization:\n");
        kprintf("  Total: %d MB\n", (int)total);
        kprintf("  Used:  %d MB\n", (int)(total - free));
        kprintf("  Free:  %d MB\n", (int)free);
    } else if (strcmp(cmdline, "date") == 0) {
        rtc_time_t now;
        rtc_get_time(&now);
        kprintf("%d-%d-%d %d:%d:%d UTC\n", (int)now.year, (int)now.month, (int)now.day, (int)now.hour, (int)now.minute, (int)now.second);
    } else if (strncmp(cmdline, "echo ", 5) == 0) {
        kprintf("%s\n", cmdline + 5);
    } else if (strcmp(cmdline, "reboot") == 0) {
        kprintf("Rebooting system...\n");
        outb(0x64, 0xFE); // Pulse reset line via keyboard controller
    } else if (strcmp(cmdline, "poweroff") == 0) {
        kprintf("Powering off...\n");
        outw(0x604, 0x2000); // QEMU ACPI poweroff
        outw(0xB004, 0x2000);
    } else {
        kprintf("chef-sh: command not found: %s (type 'help' for commands)\n", cmdline);
    }
}

void chef_shell_handle_char(char c) {
    if (c == '\n') {
        kprintf("\n");
        s_input_buf[s_input_len] = '\0';
        chef_shell_execute_cmd(s_input_buf);
        s_input_len = 0;
        kprintf("chef-os> ");
        return;
    }
    if (c == '\b') {
        if (s_input_len > 0) {
            s_input_len--;
            s_input_buf[s_input_len] = '\0';
            kprintf("\b \b");
        }
        return;
    }
    if (s_input_len < 254 && c >= ' ' && c <= '~') {
        s_input_buf[s_input_len++] = c;
        kprintf("%c", c);
    }
}
