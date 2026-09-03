#ifndef CHEF_VFS_H
#define CHEF_VFS_H

#include "types.h"

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06

#define O_RDONLY  0x0000
#define O_WRONLY  0x0001
#define O_RDWR    0x0002
#define O_CREAT   0x0040

struct vfs_node;

typedef int64_t (*vfs_read_t)(struct vfs_node *node, uint64_t offset, size_t size, void *buffer);
typedef int64_t (*vfs_write_t)(struct vfs_node *node, uint64_t offset, size_t size, const void *buffer);
typedef void (*vfs_open_t)(struct vfs_node *node);
typedef void (*vfs_close_t)(struct vfs_node *node);
typedef struct vfs_node *(*vfs_finddir_t)(struct vfs_node *node, const char *name);

typedef struct vfs_node {
    char name[128];
    uint32_t flags;
    uint64_t length;
    uint32_t inode;
    vfs_read_t read;
    vfs_write_t write;
    vfs_open_t open;
    vfs_close_t close;
    vfs_finddir_t finddir;
    struct vfs_node *ptr; // Internal filesystem pointer
} vfs_node_t;

typedef struct {
    vfs_node_t *node;
    uint64_t offset;
    int flags;
} file_desc_t;

void vfs_init(void);
vfs_node_t *vfs_lookup(const char *path);
int vfs_open(const char *path, int flags);
void vfs_close(int fd);
int64_t vfs_read(int fd, void *buffer, size_t count);
int64_t vfs_write(int fd, const void *buffer, size_t count);

extern vfs_node_t *g_vfs_root;

#endif
