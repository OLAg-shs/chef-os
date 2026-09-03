#include "fs/vfs.h"
#include "mm/heap.h"
#include "string.h"
#include "kprintf.h"

vfs_node_t *g_vfs_root = NULL;
static file_desc_t s_fd_table[64];

void vfs_init(void) {
    memset(s_fd_table, 0, sizeof(s_fd_table));
}

vfs_node_t *vfs_lookup(const char *path) {
    if (!path || !g_vfs_root) return NULL;
    if (strcmp(path, "/") == 0) return g_vfs_root;

    if (*path == '/') path++;

    char component[128];
    vfs_node_t *curr = g_vfs_root;

    while (*path) {
        int i = 0;
        while (*path && *path != '/' && i < 127) {
            component[i++] = *path++;
        }
        component[i] = '\0';
        if (*path == '/') path++;

        if (curr->finddir) {
            curr = curr->finddir(curr, component);
            if (!curr) return NULL;
        } else {
            return NULL;
        }
    }
    return curr;
}

int vfs_open(const char *path, int flags) {
    (void)flags;
    vfs_node_t *node = vfs_lookup(path);
    if (!node) return -1;

    for (int i = 0; i < 64; i++) {
        if (s_fd_table[i].node == NULL) {
            s_fd_table[i].node = node;
            s_fd_table[i].offset = 0;
            s_fd_table[i].flags = flags;
            if (node->open) node->open(node);
            return i;
        }
    }
    return -1;
}

void vfs_close(int fd) {
    if (fd < 0 || fd >= 64 || !s_fd_table[fd].node) return;
    if (s_fd_table[fd].node->close) {
        s_fd_table[fd].node->close(s_fd_table[fd].node);
    }
    s_fd_table[fd].node = NULL;
}

int64_t vfs_read(int fd, void *buffer, size_t count) {
    if (fd < 0 || fd >= 64 || !s_fd_table[fd].node) return -1;
    vfs_node_t *node = s_fd_table[fd].node;
    if (!node->read) return -1;

    int64_t bytes = node->read(node, s_fd_table[fd].offset, count, buffer);
    if (bytes > 0) {
        s_fd_table[fd].offset += bytes;
    }
    return bytes;
}

int64_t vfs_write(int fd, const void *buffer, size_t count) {
    if (fd < 0 || fd >= 64 || !s_fd_table[fd].node) return -1;
    vfs_node_t *node = s_fd_table[fd].node;
    if (!node->write) return -1;

    int64_t bytes = node->write(node, s_fd_table[fd].offset, count, buffer);
    if (bytes > 0) {
        s_fd_table[fd].offset += bytes;
    }
    return bytes;
}
