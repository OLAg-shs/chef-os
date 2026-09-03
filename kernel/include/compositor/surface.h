#ifndef CHEF_SURFACE_H
#define CHEF_SURFACE_H

#include "types.h"
#include "video/fb.h"

#define SURFACE_FLAG_VISIBLE   (1 << 0)
#define SURFACE_FLAG_FOCUSED   (1 << 1)
#define SURFACE_FLAG_PANEL     (1 << 2)
#define SURFACE_FLAG_WINDOW    (1 << 3)
#define SURFACE_FLAG_ISLAND    (1 << 4)

typedef struct surface {
    uint32_t id;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t *pixels;
    uint32_t flags;
    int32_t z_order;
    char title[64];
    struct surface *next;
} surface_t;

surface_t *surface_create(uint32_t width, uint32_t height, uint32_t flags, const char *title);
void surface_destroy(surface_t *surf);
void surface_set_position(surface_t *surf, int32_t x, int32_t y);
void surface_clear(surface_t *surf, uint32_t color);
void surface_draw_rect(surface_t *surf, int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color);
void surface_draw_string(surface_t *surf, const char *str, int32_t x, int32_t y, uint32_t fg);

#endif
