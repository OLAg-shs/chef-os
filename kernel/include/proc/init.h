#ifndef CHEF_INIT_H
#define CHEF_INIT_H

#include "types.h"

typedef struct {
    const char *name;
    const char *description;
    uint32_t pid;
    bool active;
} service_t;

void init_system_boot(void);
void init_list_services(void);

#endif
