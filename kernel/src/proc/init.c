#include "proc/init.h"
#include "proc/process.h"
#include "kprintf.h"
#include "string.h"

static service_t s_services[] = {
    {"klogd",      "Kernel System Logging Daemon",      0, false},
    {"compositor", "Chef Native 2D Surface Compositor", 0, false},
    {"chef-sh",    "Interactive Parchment Shell",       0, false}
};

void init_system_boot(void) {
    // PID 1: init service supervisor
    process_t *init_proc = process_create("init", 0);
    (void)init_proc;

    for (size_t i = 0; i < sizeof(s_services) / sizeof(s_services[0]); i++) {
        process_t *svc_proc = process_create(s_services[i].name, 1);
        s_services[i].pid = svc_proc->pid;
        s_services[i].active = true;
    }
}

void init_list_services(void) {
    kprintf("  SERVICE       PID    STATUS    DESCRIPTION\n");
    kprintf("  --------------------------------------------------------\n");
    for (size_t i = 0; i < sizeof(s_services) / sizeof(s_services[0]); i++) {
        const char *status = s_services[i].active ? "ACTIVE  " : "INACTIVE";
        kprintf("  %s (%d) [%s] - %s\n",
                s_services[i].name, (int)s_services[i].pid, status, s_services[i].description);
    }
}
