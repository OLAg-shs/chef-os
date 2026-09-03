#include "proc/sched.h"
#include "mm/heap.h"
#include "mm/pmm.h"
#include "string.h"
#include "kprintf.h"

static thread_t *s_current_thread = NULL;
static thread_t *s_thread_list = NULL;
static uint64_t s_next_id = 1;

void sched_init(void) {
    // Main kernel bootstrap thread
    s_current_thread = (thread_t *)kzalloc(sizeof(thread_t));
    s_current_thread->id = s_next_id++;
    s_current_thread->state = THREAD_RUNNING;
    strcpy(s_current_thread->name, "kmain");
    s_current_thread->next = s_current_thread;
    s_thread_list = s_current_thread;
}

thread_t *thread_create(void (*entry)(void), const char *name) {
    thread_t *t = (thread_t *)kzalloc(sizeof(thread_t));
    t->id = s_next_id++;
    t->state = THREAD_READY;
    t->rip = (uint64_t)entry;
    strncpy(t->name, name, 31);

    // Allocate 16KB stack
    void *stack_phys = pmm_alloc_pages(4);
    t->rsp = (uint64_t)stack_phys + (4 * PAGE_SIZE) - 16;

    // Link into round-robin ring
    t->next = s_thread_list->next;
    s_thread_list->next = t;

    return t;
}

void sched_yield(void) {
    // Cooperative yield stub (preemption activated via timer ISR)
    if (s_current_thread && s_current_thread->next) {
        s_current_thread = s_current_thread->next;
    }
}
