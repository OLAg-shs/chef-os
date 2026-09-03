#include "compositor/surface.h"
#include "mm/heap.h"
#include "string.h"

extern const uint8_t font_8x16[128][16];
static uint32_t s_next_surface_id = 1;

surface_t *surface_create(uint32_t width, uint32_t height, uint32_t flags, const char *title) {
    surface_t *surf = (surface_t *)kzalloc(sizeof(surface_t));
    if (!surf) return NULL;

    surf->id = s_next_surface_id++;
    surf->width = width;
    surf->height = height;
    surf->flags = flags | SURFACE_FLAG_VISIBLE;
    surf->pixels = (uint32_t *)kzalloc(width * height * sizeof(uint32_t));
    if (title) strncpy(surf->title, title, 63);

    return surf;
}

void surface_destroy(surface_t *surf) {
    if (!surf) return;
    if (surf->pixels) kfree(surf->pixels);
    kfree(surf);
}

void surface_set_position(surface_t *surf, int32_t x, int32_t y) {
    if (!surf) return;
    surf->x = x;
    surf->y = y;
}

void surface_clear(surface_t *surf, uint32_t color) {
    if (!surf || !surf->pixels) return;
    uint32_t count = surf->width * surf->height;
    for (uint32_t i = 0; i < count; i++) {
        surf->pixels[i] = color;
    }
}

void surface_draw_rect(surface_t *surf, int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (!surf || !surf->pixels) return;
    for (uint32_t j = y; j < y + h && j < surf->height; j++) {
        for (uint32_t i = x; i < x + w && i < surf->width; i++) {
            surf->pixels[j * surf->width + i] = color;
        }
    }
}

void surface_draw_string(surface_t *surf, const char *str, int32_t x, int32_t y, uint32_t fg) {
    if (!surf || !surf->pixels || !str) return;
    while (*str) {
        char c = *str++;
        if ((uint8_t)c > 127) c = '?';
        const uint8_t *glyph = font_8x16[(uint8_t)c];
        for (int r = 0; r < 16; r++) {
            uint8_t bits = glyph[r];
            for (int col = 0; col < 8; col++) {
                if ((bits >> (7 - col)) & 1) {
                    int32_t px = x + col;
                    int32_t py = y + r;
                    if (px >= 0 && px < (int32_t)surf->width && py >= 0 && py < (int32_t)surf->height) {
                        surf->pixels[py * surf->width + px] = fg;
                    }
                }
            }
        }
        x += 8;
    }
}
