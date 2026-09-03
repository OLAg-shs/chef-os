#ifndef CHEF_PS2_MOUSE_H
#define CHEF_PS2_MOUSE_H

#include "types.h"

typedef struct {
    int32_t x;
    int32_t y;
    bool left_button;
    bool right_button;
    bool middle_button;
} mouse_state_t;

void ps2_mouse_init(uint32_t screen_width, uint32_t screen_height);
void ps2_mouse_get_state(mouse_state_t *state);

#endif
