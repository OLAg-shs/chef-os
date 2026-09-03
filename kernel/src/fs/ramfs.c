#include "fs/ramfs.h"
#include "mm/heap.h"
#include "string.h"

typedef struct ramfs_entry {
    vfs_node_t vfs_node;
    uint8_t *data;
    size_t capacity;
    struct ramfs_entry *children[32];
    size_t child_count;
} ramfs_entry_t;

static int64_t ramfs_read(vfs_node_t *node, uint64_t offset, size_t size, void *buffer) {
    ramfs_entry_t *entry = (ramfs_entry_t *)node;
    if (offset >= entry->vfs_node.length) return 0;
    if (offset + size > entry->vfs_node.length) {
        size = entry->vfs_node.length - offset;
    }
    memcpy(buffer, entry->data + offset, size);
    return (int64_t)size;
}

static int64_t ramfs_write(vfs_node_t *node, uint64_t offset, size_t size, const void *buffer) {
    ramfs_entry_t *entry = (ramfs_entry_t *)node;
    if (offset + size > entry->capacity) {
        size_t new_cap = (offset + size + 511) & ~511;
        uint8_t *new_data = (uint8_t *)kmalloc(new_cap);
        if (entry->data) {
            memcpy(new_data, entry->data, entry->vfs_node.length);
            kfree(entry->data);
        }
        entry->data = new_data;
        entry->capacity = new_cap;
    }
    memcpy(entry->data + offset, buffer, size);
    if (offset + size > entry->vfs_node.length) {
        entry->vfs_node.length = offset + size;
    }
    return (int64_t)size;
}

static vfs_node_t *ramfs_finddir(vfs_node_t *node, const char *name) {
    ramfs_entry_t *entry = (ramfs_entry_t *)node;
    for (size_t i = 0; i < entry->child_count; i++) {
        if (strcmp(entry->children[i]->vfs_node.name, name) == 0) {
            return &entry->children[i]->vfs_node;
        }
    }
    return NULL;
}

vfs_node_t *ramfs_create_root(void) {
    ramfs_entry_t *root = (ramfs_entry_t *)kzalloc(sizeof(ramfs_entry_t));
    strcpy(root->vfs_node.name, "/");
    root->vfs_node.flags = FS_DIRECTORY;
    root->vfs_node.finddir = ramfs_finddir;
    return &root->vfs_node;
}

vfs_node_t *ramfs_create_dir(vfs_node_t *parent, const char *name) {
    if (!parent || !(parent->flags & FS_DIRECTORY)) return NULL;
    ramfs_entry_t *p = (ramfs_entry_t *)parent;
    if (p->child_count >= 32) return NULL;

    ramfs_entry_t *dir = (ramfs_entry_t *)kzalloc(sizeof(ramfs_entry_t));
    strncpy(dir->vfs_node.name, name, 127);
    dir->vfs_node.flags = FS_DIRECTORY;
    dir->vfs_node.finddir = ramfs_finddir;

    p->children[p->child_count++] = dir;
    return &dir->vfs_node;
}

vfs_node_t *ramfs_create_file(vfs_node_t *parent, const char *name, const void *content, size_t size) {
    if (!parent || !(parent->flags & FS_DIRECTORY)) return NULL;
    ramfs_entry_t *p = (ramfs_entry_t *)parent;
    if (p->child_count >= 32) return NULL;

    ramfs_entry_t *file = (ramfs_entry_t *)kzalloc(sizeof(ramfs_entry_t));
    strncpy(file->vfs_node.name, name, 127);
    file->vfs_node.flags = FS_FILE;
    file->vfs_node.read = ramfs_read;
    file->vfs_node.write = ramfs_write;

    if (size > 0 && content) {
        file->data = (uint8_t *)kmalloc(size);
        memcpy(file->data, content, size);
        file->capacity = size;
        file->vfs_node.length = size;
    }

    p->children[p->child_count++] = file;
    return &file->vfs_node;
}
