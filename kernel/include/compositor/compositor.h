#ifndef CHEF_COMPOSITOR_H
#define CHEF_COMPOSITOR_H

#include "compositor/surface.h"
#include "compositor/window.h"

typedef struct {
    uint32_t screen_width;
    uint32_t screen_height;
    uint8_t active_workspace;
    window_t *window_list;
    window_t *focused_window;
    int32_t mouse_x;
    int32_t mouse_y;
    bool needs_redraw;
} compositor_state_t;

void compositor_init(void);
void compositor_register_window(window_t *win);
void compositor_render_frame(void);
void compositor_set_mouse(int32_t x, int32_t y);
void compositor_set_workspace(uint8_t ws);
void compositor_cycle_focus(void);
compositor_state_t *compositor_get_state(void);

#endif
