#ifndef CHEF_SYSCALL_H
#define CHEF_SYSCALL_H

#include "types.h"

#define SYS_EXIT         1
#define SYS_FORK         2
#define SYS_READ         3
#define SYS_WRITE        4
#define SYS_OPEN         5
#define SYS_CLOSE        6
#define SYS_GETPID       7
#define SYS_YIELD        8
#define SYS_MMAP         9
#define SYS_MUNMAP       10
#define SYS_DRAW_RECT    11
#define SYS_SWAP_BUFFERS 12
#define SYS_GET_TIME     13

void syscall_init(uint64_t hhdm_offset);
int64_t syscall_dispatch(uint64_t num, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6);
void user_mode_enter_test(void);

#endif
