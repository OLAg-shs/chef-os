#include "fs/devfs.h"
#include "fs/ramfs.h"
#include "drivers/serial.h"
#include "drivers/keyboard.h"
#include "video/fb.h"
#include "mm/heap.h"
#include "string.h"

static int64_t dev_serial_write(vfs_node_t *node, uint64_t offset, size_t size, const void *buffer) {
    (void)node; (void)offset;
    const char *s = (const char *)buffer;
    for (size_t i = 0; i < size; i++) {
        serial_putc(s[i]);
    }
    return (int64_t)size;
}

static int64_t dev_null_write(vfs_node_t *node, uint64_t offset, size_t size, const void *buffer) {
    (void)node; (void)offset; (void)buffer;
    return (int64_t)size;
}

static int64_t dev_zero_read(vfs_node_t *node, uint64_t offset, size_t size, void *buffer) {
    (void)node; (void)offset;
    memset(buffer, 0, size);
    return (int64_t)size;
}

static int64_t dev_kbd_read(vfs_node_t *node, uint64_t offset, size_t size, void *buffer) {
    (void)node; (void)offset;
    char *b = (char *)buffer;
    if (size > 0) {
        char c = keyboard_get_last_char();
        if (c) {
            b[0] = c;
            return 1;
        }
    }
    return 0;
}

void devfs_init(vfs_node_t *dev_dir) {
    if (!dev_dir) return;

    vfs_node_t *s = ramfs_create_file(dev_dir, "serial", NULL, 0);
    if (s) { s->flags = FS_CHARDEVICE; s->write = dev_serial_write; }

    vfs_node_t *n = ramfs_create_file(dev_dir, "null", NULL, 0);
    if (n) { n->flags = FS_CHARDEVICE; n->write = dev_null_write; }

    vfs_node_t *z = ramfs_create_file(dev_dir, "zero", NULL, 0);
    if (z) { z->flags = FS_CHARDEVICE; z->read = dev_zero_read; }

    vfs_node_t *k = ramfs_create_file(dev_dir, "kbd", NULL, 0);
    if (k) { k->flags = FS_CHARDEVICE; k->read = dev_kbd_read; }
}
