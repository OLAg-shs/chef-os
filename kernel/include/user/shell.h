#ifndef CHEF_SHELL_H
#define CHEF_SHELL_H

#include "types.h"

void chef_shell_init(void);
void chef_shell_handle_char(char c);
void chef_shell_execute_cmd(const char *cmdline);

#endif
