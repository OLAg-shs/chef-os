#include "compositor/shell_ui.h"
#include "video/fb.h"
#include "drivers/rtc.h"
#include "drivers/timer.h"
#include "string.h"

static shell_ui_state_t s_shell;

void shell_ui_init(void) {
    s_shell.island_state = ISLAND_IDLE;
    s_shell.island_msg[0] = '\0';
    s_shell.toast_expiry_tick = 0;
    s_shell.launcher_open = false;
}

void shell_ui_show_toast(const char *msg, uint32_t duration_ms) {
    if (!msg) return;
    strncpy(s_shell.island_msg, msg, 127);
    s_shell.island_state = ISLAND_TOAST;
    s_shell.toast_expiry_tick = timer_get_ticks() + duration_ms;
}

void shell_ui_render_dock(uint32_t screen_h, uint8_t active_ws) {
    uint32_t dock_x = 12;
    uint32_t dock_y = 12;
    uint32_t dock_w = 56;
    uint32_t dock_h = screen_h - 24;

    fb_draw_rounded_rect(dock_x, dock_y, dock_w, dock_h, 16, COLOR_SURFACE, COLOR_BORDER);

    // Crest Logo
    fb_draw_rounded_rect(dock_x + 10, dock_y + 12, 36, 36, 8, COLOR_ACCENT, 0);
    fb_draw_string("C", dock_x + 24, dock_y + 22, COLOR_WHITE, 0);

    // Workspace Switcher Pills
    uint32_t ws_y = dock_y + 60;
    const char *ws_labels[] = {"1", "2", "3", "4", "+"};
    for (int i = 0; i < 5; i++) {
        uint32_t pill_bg = (i + 1 == active_ws) ? COLOR_ACCENT : COLOR_SURFACE_ALT;
        uint32_t pill_fg = (i + 1 == active_ws) ? COLOR_WHITE : COLOR_TEXT;
        fb_draw_rounded_rect(dock_x + 14, ws_y + (i * 38), 28, 28, 6, pill_bg, COLOR_BORDER);
        fb_draw_string(ws_labels[i], dock_x + 24, ws_y + (i * 38) + 6, pill_fg, 0);
    }

    // App Quick-Launchers
    uint32_t app_y = ws_y + (5 * 38) + 12;
    // Terminal Launcher [>_]
    fb_draw_rounded_rect(dock_x + 14, app_y, 28, 28, 6, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string(">", dock_x + 24, app_y + 6, COLOR_TEXT, 0);

    // Monitor Launcher [#]
    fb_draw_rounded_rect(dock_x + 14, app_y + 36, 28, 28, 6, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string("#", dock_x + 24, app_y + 42, COLOR_TEXT, 0);

    // Files Launcher [/]
    fb_draw_rounded_rect(dock_x + 14, app_y + 72, 28, 28, 6, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string("/", dock_x + 24, app_y + 78, COLOR_TEXT, 0);

    // Bottom Controls
    uint32_t bot_y = dock_y + dock_h - 130;
    fb_draw_rounded_rect(dock_x + 14, bot_y, 28, 28, 6, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string("v", dock_x + 24, bot_y + 6, COLOR_TEXT_MUTED, 0);

    fb_draw_rounded_rect(dock_x + 14, bot_y + 36, 28, 28, 6, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string("w", dock_x + 24, bot_y + 42, COLOR_TEXT_MUTED, 0);

    fb_draw_rounded_rect(dock_x + 14, bot_y + 72, 28, 28, 6, COLOR_ACCENT, 0);
    fb_draw_string("x", dock_x + 24, bot_y + 78, COLOR_WHITE, 0);
}

void shell_ui_render_island(uint32_t screen_w, const char *focused_title, const char *layout_name) {
    // Check toast expiry
    if (s_shell.island_state == ISLAND_TOAST && timer_get_ticks() >= s_shell.toast_expiry_tick) {
        s_shell.island_state = ISLAND_IDLE;
    }

    uint32_t island_w = (s_shell.island_state == ISLAND_TOAST) ? 440 : 380;
    uint32_t island_h = 36;
    uint32_t island_x = (screen_w - island_w) / 2;
    uint32_t island_y = 12;

    uint32_t island_bg = (s_shell.island_state == ISLAND_TOAST) ? COLOR_SURFACE_ALT : COLOR_SURFACE;
    fb_draw_rounded_rect(island_x, island_y, island_w, island_h, 18, island_bg, COLOR_BORDER);

    if (s_shell.island_state == ISLAND_TOAST) {
        fb_draw_string(s_shell.island_msg, island_x + 24, island_y + 10, COLOR_ACCENT, 0);
    } else {
        rtc_time_t now;
        rtc_get_time(&now);
        char buf[128];
        const char *win_name = (focused_title && *focused_title) ? focused_title : "Chef OS";

        // Format: "<Title>  [<Layout>]  ::  <HH:MM>"
        char time_str[16];
        time_str[0] = '0' + (now.hour / 10);
        time_str[1] = '0' + (now.hour % 10);
        time_str[2] = ':';
        time_str[3] = '0' + (now.minute / 10);
        time_str[4] = '0' + (now.minute % 10);
        time_str[5] = '\0';

        char win_short[24];
        strncpy(win_short, win_name, 18);
        win_short[18] = '\0';

        strcpy(buf, win_short);
        strcat(buf, " [");
        strcat(buf, layout_name ? layout_name : "Tile");
        strcat(buf, "]  ::  ");
        strcat(buf, time_str);

        fb_draw_string(buf, island_x + 24, island_y + 10, COLOR_TEXT, 0);
    }
}
