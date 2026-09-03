#include "compositor/compositor.h"
#include "video/fb.h"
#include "drivers/rtc.h"
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

static void draw_dock(void) {
    uint32_t dock_x = 12;
    uint32_t dock_y = 12;
    uint32_t dock_w = 56;
    uint32_t dock_h = s_comp.screen_height - 24;

    fb_draw_rounded_rect(dock_x, dock_y, dock_w, dock_h, 16, COLOR_SURFACE, COLOR_BORDER);

    // Crest Icon
    fb_draw_rounded_rect(dock_x + 10, dock_y + 12, 36, 36, 8, COLOR_ACCENT, 0);
    fb_draw_string("C", dock_x + 24, dock_y + 22, COLOR_WHITE, 0);

    // Workspace Pills
    uint32_t ws_y = dock_y + 60;
    const char *ws_labels[] = {"1", "2", "3", "4", "+"};
    for (int i = 0; i < 5; i++) {
        uint32_t pill_bg = (i + 1 == s_comp.active_workspace) ? COLOR_ACCENT : COLOR_SURFACE_ALT;
        uint32_t pill_fg = (i + 1 == s_comp.active_workspace) ? COLOR_WHITE : COLOR_TEXT;
        fb_draw_rounded_rect(dock_x + 14, ws_y + (i * 38), 28, 28, 6, pill_bg, COLOR_BORDER);
        fb_draw_string(ws_labels[i], dock_x + 24, ws_y + (i * 38) + 6, pill_fg, 0);
    }

    // Bottom Controls
    uint32_t bot_y = dock_y + dock_h - 130;
    fb_draw_rounded_rect(dock_x + 14, bot_y, 28, 28, 6, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string("v", dock_x + 24, bot_y + 6, COLOR_TEXT_MUTED, 0);

    fb_draw_rounded_rect(dock_x + 14, bot_y + 36, 28, 28, 6, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string("w", dock_x + 24, bot_y + 42, COLOR_TEXT_MUTED, 0);

    fb_draw_rounded_rect(dock_x + 14, bot_y + 72, 28, 28, 6, COLOR_ACCENT, 0);
    fb_draw_string("x", dock_x + 24, bot_y + 78, COLOR_WHITE, 0);
}

static void draw_dynamic_island(void) {
    uint32_t island_w = 340;
    uint32_t island_h = 36;
    uint32_t island_x = (s_comp.screen_width - island_w) / 2;
    uint32_t island_y = 12;

    fb_draw_rounded_rect(island_x, island_y, island_w, island_h, 18, COLOR_SURFACE, COLOR_BORDER);

    rtc_time_t now;
    rtc_get_time(&now);
    char buf[64];
    const char *win_name = s_comp.focused_window ? s_comp.focused_window->title : "Chef OS";

    // Format top pill string
    strcpy(buf, win_name);
    strcat(buf, "  ::  ");
    char time_str[16];
    time_str[0] = '0' + (now.hour / 10);
    time_str[1] = '0' + (now.hour % 10);
    time_str[2] = ':';
    time_str[3] = '0' + (now.minute / 10);
    time_str[4] = '0' + (now.minute % 10);
    time_str[5] = '\0';
    strcat(buf, time_str);

    fb_draw_string(buf, island_x + 36, island_y + 10, COLOR_TEXT, 0);
}

static void draw_window(window_t *win) {
    if (!win || win->is_minimized) return;

    // Window Frame / Shadow
    fb_draw_rounded_rect(win->x, win->y, win->width, win->height, 12, COLOR_SURFACE, COLOR_BORDER);

    // Titlebar
    uint32_t tb_bg = win->is_focused ? COLOR_SURFACE_ALT : COLOR_SURFACE;
    fb_draw_rounded_rect(win->x, win->y, win->width, WINDOW_TITLEBAR_HEIGHT, 12, tb_bg, COLOR_BORDER);

    // Title text
    fb_draw_string(win->title, win->x + 14, win->y + 6, COLOR_TEXT, 0);

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

    // 2. Render Left Dock & Dynamic Island
    draw_dock();
    draw_dynamic_island();

    // 3. Render Windows in Z-order
    window_t *curr = s_comp.window_list;
    while (curr) {
        draw_window(curr);
        curr = curr->next;
    }

    // 4. Render Mouse Cursor
    fb_draw_cursor(s_comp.mouse_x, s_comp.mouse_y);

    // 5. Present frame to GOP Framebuffer
    fb_swap();
    s_comp.needs_redraw = false;
}
