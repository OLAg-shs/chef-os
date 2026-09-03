#ifndef CHEF_SHELL_UI_H
#define CHEF_SHELL_UI_H

#include "types.h"

typedef enum {
    ISLAND_IDLE,
    ISLAND_AUDIO,
    ISLAND_TOAST
} island_state_t;

typedef struct {
    island_state_t island_state;
    char island_msg[128];
    uint64_t toast_expiry_tick;
    bool launcher_open;
} shell_ui_state_t;

void shell_ui_init(void);
void shell_ui_show_toast(const char *msg, uint32_t duration_ms);
void shell_ui_render_dock(uint32_t screen_h, uint8_t active_ws);
void shell_ui_render_island(uint32_t screen_w, const char *focused_title, const char *layout_name);

#endif
