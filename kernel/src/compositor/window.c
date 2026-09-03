#include "compositor/window.h"
#include "mm/heap.h"
#include "string.h"

static uint32_t s_next_window_id = 1;

window_t *window_create(int32_t x, int32_t y, uint32_t w, uint32_t h, const char *title) {
    window_t *win = (window_t *)kzalloc(sizeof(window_t));
    if (!win) return NULL;

    win->id = s_next_window_id++;
    win->x = x;
    win->y = y;
    win->width = w;
    win->height = h;
    win->workspace_id = 1;
    win->is_focused = true;
    if (title) strncpy(win->title, title, 63);

    // Create client content surface
    win->surface = surface_create(w - (2 * WINDOW_BORDER_WIDTH),
                                  h - WINDOW_TITLEBAR_HEIGHT - WINDOW_BORDER_WIDTH,
                                  SURFACE_FLAG_WINDOW,
                                  title);
    return win;
}

void window_destroy(window_t *win) {
    if (!win) return;
    if (win->surface) surface_destroy(win->surface);
    kfree(win);
}

void window_set_focus(window_t *win, bool focused) {
    if (!win) return;
    win->is_focused = focused;
}
