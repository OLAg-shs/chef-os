#include "mm/heap.h"
#include "mm/pmm.h"
#include "string.h"

typedef struct heap_block {
    size_t size;
    bool is_free;
    struct heap_block *next;
} heap_block_t;

static uint8_t *s_heap_start = NULL;
static heap_block_t *s_head = NULL;
static uint64_t s_hhdm = 0;

void heap_init(uint64_t hhdm_offset) {
    s_hhdm = hhdm_offset;
    size_t initial_pages = 8192; // 32 MB initial kernel heap
    void *phys = pmm_alloc_pages(initial_pages);
    s_heap_start = (uint8_t *)((uint64_t)phys + s_hhdm);

    s_head = (heap_block_t *)s_heap_start;
    s_head->size = (initial_pages * PAGE_SIZE) - sizeof(heap_block_t);
    s_head->is_free = true;
    s_head->next = NULL;
}

void *kmalloc(size_t size) {
    size = (size + 7) & ~7; // 8-byte align
    heap_block_t *curr = s_head;

    while (curr) {
        if (curr->is_free && curr->size >= size) {
            if (curr->size >= size + sizeof(heap_block_t) + 32) {
                // Split block
                heap_block_t *new_block = (heap_block_t *)((uint8_t *)curr + sizeof(heap_block_t) + size);
                new_block->size = curr->size - size - sizeof(heap_block_t);
                new_block->is_free = true;
                new_block->next = curr->next;

                curr->size = size;
                curr->is_free = false;
                curr->next = new_block;
            } else {
                curr->is_free = false;
            }
            return (void *)((uint8_t *)curr + sizeof(heap_block_t));
        }
        curr = curr->next;
    }
    return NULL;
}

void *kzalloc(size_t size) {
    void *p = kmalloc(size);
    if (p) memset(p, 0, size);
    return p;
}

void kfree(void *ptr) {
    if (!ptr) return;
    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    block->is_free = true;

    // Merge consecutive free blocks
    heap_block_t *curr = s_head;
    while (curr && curr->next) {
        if (curr->is_free && curr->next->is_free) {
            curr->size += sizeof(heap_block_t) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}
