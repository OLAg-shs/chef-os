#ifndef CHEF_PROCESS_H
#define CHEF_PROCESS_H

#include "types.h"

typedef enum {
    PROC_READY,
    PROC_RUNNING,
    PROC_SLEEPING,
    PROC_ZOMBIE,
    PROC_STOPPED
} proc_state_t;

typedef struct process {
    uint32_t pid;
    uint32_t ppid;
    char name[32];
    proc_state_t state;
    uint64_t memory_used;
    struct process *next;
} process_t;

void process_init(void);
process_t *process_create(const char *name, uint32_t ppid);
void process_list(void);

#endif
