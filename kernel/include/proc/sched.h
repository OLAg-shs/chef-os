#ifndef CHEF_SCHED_H
#define CHEF_SCHED_H

#include "types.h"

typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_TERMINATED
} thread_state_t;

typedef struct thread {
    uint64_t rsp;
    uint64_t rip;
    uint64_t id;
    thread_state_t state;
    char name[32];
    struct thread *next;
} thread_t;

void sched_init(void);
thread_t *thread_create(void (*entry)(void), const char *name);
void sched_yield(void);

#endif
