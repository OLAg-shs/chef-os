#include "compositor/compositor.h"
#include "compositor/layout.h"
#include "compositor/shell_ui.h"
#include "video/fb.h"
#include "string.h"
#include "kprintf.h"

static compositor_state_t s_comp;

void compositor_init(void) {
    fb_info_t *fb = fb_get_info();
    s_comp.screen_width = fb ? (uint32_t)fb->width : 1280;
    s_comp.screen_height = fb ? (uint32_t)fb->height : 800;
    s_comp.active_workspace = 1;
    s_comp.window_list = NULL;
    s_comp.focused_window = NULL;
    s_comp.mouse_x = s_comp.screen_width / 2;
    s_comp.mouse_y = s_comp.screen_height / 2;
    s_comp.needs_redraw = true;

    layout_init();
    shell_ui_init();
}

void compositor_register_window(window_t *win) {
    if (!win) return;
    win->next = s_comp.window_list;
    s_comp.window_list = win;
    s_comp.focused_window = win;
    s_comp.needs_redraw = true;
}

void compositor_set_mouse(int32_t x, int32_t y) {
    s_comp.mouse_x = x;
    s_comp.mouse_y = y;
}

compositor_state_t *compositor_get_state(void) {
    return &s_comp;
}

void compositor_set_workspace(uint8_t ws) {
    if (ws >= 1 && ws <= 4) {
        s_comp.active_workspace = ws;
        s_comp.needs_redraw = true;
    }
}

void compositor_cycle_focus(void) {
    if (!s_comp.window_list) return;
    if (s_comp.focused_window && s_comp.focused_window->next) {
        s_comp.focused_window = s_comp.focused_window->next;
    } else {
        s_comp.focused_window = s_comp.window_list;
    }
    s_comp.needs_redraw = true;
}

static void draw_window(window_t *win) {
    if (!win || win->is_minimized || win->workspace_id != s_comp.active_workspace) return;

    // Window Frame
    fb_draw_rounded_rect(win->x, win->y, win->width, win->height, 12, COLOR_SURFACE, COLOR_BORDER);

    // Titlebar
    uint32_t tb_bg = (win == s_comp.focused_window) ? COLOR_SURFACE_ALT : COLOR_SURFACE;
    fb_draw_rounded_rect(win->x, win->y, win->width, WINDOW_TITLEBAR_HEIGHT, 12, tb_bg, COLOR_BORDER);

    // Title text
    uint32_t title_fg = (win == s_comp.focused_window) ? COLOR_TEXT : COLOR_TEXT_MUTED;
    fb_draw_string(win->title, win->x + 14, win->y + 6, title_fg, 0);

    // Window Close Pill (Right side)
    uint32_t close_x = win->x + win->width - 24;
    uint32_t close_y = win->y + 7;
    fb_draw_rounded_rect(close_x, close_y, 14, 14, 4, COLOR_ACCENT, 0);
    fb_draw_string("x", close_x + 4, close_y + 1, COLOR_WHITE, 0);

    // Blit Window Content Surface
    if (win->surface && win->surface->pixels) {
        uint32_t content_x = win->x + WINDOW_BORDER_WIDTH;
        uint32_t content_y = win->y + WINDOW_TITLEBAR_HEIGHT;
        uint32_t sw = win->surface->width;
        uint32_t sh = win->surface->height;

        for (uint32_t j = 0; j < sh; j++) {
            for (uint32_t i = 0; i < sw; i++) {
                uint32_t px = win->surface->pixels[j * sw + i];
                if (px != 0) {
                    fb_put_pixel(content_x + i, content_y + j, px);
                }
            }
        }
    }
}

void compositor_render_frame(void) {
    // 1. Clear background
    fb_clear(COLOR_BG);

    // 2. Apply Dynamic Tiling Layout Engine
    layout_apply(s_comp.window_list, s_comp.active_workspace, s_comp.screen_width, s_comp.screen_height);

    // 3. Render Windows in workspace
    window_t *curr = s_comp.window_list;
    while (curr) {
        if (curr != s_comp.focused_window) {
            draw_window(curr);
        }
        curr = curr->next;
    }
    if (s_comp.focused_window) {
        draw_window(s_comp.focused_window);
    }

    // 4. Render Shell Dock & Dynamic Island
    shell_ui_render_dock(s_comp.screen_height, s_comp.active_workspace);
    const char *focused_title = s_comp.focused_window ? s_comp.focused_window->title : "Chef OS";
    shell_ui_render_island(s_comp.screen_width, focused_title, layout_get_mode_name());

    // 5. Render Mouse Cursor
    fb_draw_cursor(s_comp.mouse_x, s_comp.mouse_y);

    // 6. Present to GOP Frontbuffer
    fb_swap();
    s_comp.needs_redraw = false;
}
