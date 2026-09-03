#ifndef CHEF_WINDOW_H
#define CHEF_WINDOW_H

#include "compositor/surface.h"

#define WINDOW_TITLEBAR_HEIGHT 28
#define WINDOW_BORDER_WIDTH    1

struct window;
typedef void (*window_paint_t)(struct window *win);

typedef struct window {
    uint32_t id;
    surface_t *surface;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    uint8_t workspace_id;
    bool is_focused;
    bool is_minimized;
    char title[64];
    window_paint_t paint;
    struct window *next;
} window_t;

window_t *window_create(int32_t x, int32_t y, uint32_t w, uint32_t h, const char *title);
void window_destroy(window_t *win);
void window_set_focus(window_t *win, bool focused);
void window_resize_surface(window_t *win, uint32_t new_w, uint32_t new_h);

#endif
