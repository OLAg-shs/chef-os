#ifndef CHEF_RAMFS_H
#define CHEF_RAMFS_H

#include "fs/vfs.h"

vfs_node_t *ramfs_create_root(void);
vfs_node_t *ramfs_create_dir(vfs_node_t *parent, const char *name);
vfs_node_t *ramfs_create_file(vfs_node_t *parent, const char *name, const void *content, size_t size);

#endif
