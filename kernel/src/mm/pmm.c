#include "mm/pmm.h"
#include "string.h"
#include "kprintf.h"

static uint8_t *s_bitmap = NULL;
static uint64_t s_total_pages = 0;
static uint64_t s_free_pages = 0;
static uint64_t s_highest_addr = 0;
static uint64_t s_hhdm = 0;

static inline void bitmap_set(uint64_t bit) {
    s_bitmap[bit / 8] |= (1 << (bit % 8));
}

static inline void bitmap_clear(uint64_t bit) {
    s_bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static inline bool bitmap_test(uint64_t bit) {
    return (s_bitmap[bit / 8] >> (bit % 8)) & 1;
}

void pmm_init(struct limine_memmap_response *memmap_response, uint64_t hhdm_offset) {
    s_hhdm = hhdm_offset;
    if (!memmap_response) return;

    // Find highest address
    for (uint64_t i = 0; i < memmap_response->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap_response->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            uint64_t top = entry->base + entry->length;
            if (top > s_highest_addr) {
                s_highest_addr = top;
            }
        }
    }

    s_total_pages = s_highest_addr / PAGE_SIZE;
    uint64_t bitmap_size = (s_total_pages + 7) / 8;

    // Find usable region for bitmap
    for (uint64_t i = 0; i < memmap_response->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap_response->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= bitmap_size) {
            s_bitmap = (uint8_t *)(entry->base + s_hhdm);
            memset(s_bitmap, 0xFF, bitmap_size); // Mark all as used initially
            entry->base += (bitmap_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
            entry->length -= bitmap_size;
            break;
        }
    }

    // Free usable memory regions
    s_free_pages = 0;
    for (uint64_t i = 0; i < memmap_response->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap_response->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            for (uint64_t addr = entry->base; addr < entry->base + entry->length; addr += PAGE_SIZE) {
                bitmap_clear(addr / PAGE_SIZE);
                s_free_pages++;
            }
        }
    }
}

void *pmm_alloc_page(void) {
    for (uint64_t i = 0; i < s_total_pages; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            s_free_pages--;
            return (void *)(i * PAGE_SIZE);
        }
    }
    return NULL;
}

void *pmm_alloc_pages(size_t count) {
    uint64_t consecutive = 0;
    uint64_t start_page = 0;

    for (uint64_t i = 0; i < s_total_pages; i++) {
        if (!bitmap_test(i)) {
            if (consecutive == 0) start_page = i;
            consecutive++;
            if (consecutive == count) {
                for (uint64_t j = start_page; j < start_page + count; j++) {
                    bitmap_set(j);
                }
                s_free_pages -= count;
                return (void *)(start_page * PAGE_SIZE);
            }
        } else {
            consecutive = 0;
        }
    }
    return NULL;
}

void pmm_free_page(void *ptr) {
    uint64_t page = (uint64_t)ptr / PAGE_SIZE;
    if (page < s_total_pages && bitmap_test(page)) {
        bitmap_clear(page);
        s_free_pages++;
    }
}

void pmm_free_pages(void *ptr, size_t count) {
    uint64_t start_page = (uint64_t)ptr / PAGE_SIZE;
    for (size_t i = 0; i < count; i++) {
        if (bitmap_test(start_page + i)) {
            bitmap_clear(start_page + i);
            s_free_pages++;
        }
    }
}

uint64_t pmm_get_free_memory(void) {
    return s_free_pages * PAGE_SIZE;
}

uint64_t pmm_get_total_memory(void) {
    return s_total_pages * PAGE_SIZE;
}
