#include "proc/process.h"
#include "mm/heap.h"
#include "string.h"
#include "kprintf.h"

static process_t *s_proc_list = NULL;
static uint32_t s_next_pid = 1;

void process_init(void) {
    s_proc_list = NULL;
    s_next_pid = 1;
}

process_t *process_create(const char *name, uint32_t ppid) {
    process_t *p = (process_t *)kzalloc(sizeof(process_t));
    p->pid = s_next_pid++;
    p->ppid = ppid;
    p->state = PROC_RUNNING;
    p->memory_used = 16 * 1024;
    strncpy(p->name, name, 31);

    p->next = s_proc_list;
    s_proc_list = p;
    return p;
}

void process_list(void) {
    kprintf("  PID   PPID  STATE     MEMORY    NAME\n");
    kprintf("  -------------------------------------------\n");
    process_t *curr = s_proc_list;
    while (curr) {
        const char *state_str = (curr->state == PROC_RUNNING) ? "RUNNING" : "READY";
        kprintf("  #%d (ppid %d) [%s] %d KB : %s\n",
                (int)curr->pid, (int)curr->ppid, state_str, (int)(curr->memory_used / 1024), curr->name);
        curr = curr->next;
    }
}
