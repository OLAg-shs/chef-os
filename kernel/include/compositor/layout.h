#ifndef CHEF_LAYOUT_H
#define CHEF_LAYOUT_H

#include "types.h"
#include "compositor/window.h"

typedef enum {
    LAYOUT_MASTER_STACK,  // Master left 60%, stack right 40%
    LAYOUT_GRID,          // Even 2x2 or NxM grid
    LAYOUT_MONOCLE,       // Focused window takes full screen
    LAYOUT_FLOATING       // Freeform coordinates
} layout_mode_t;

typedef struct {
    layout_mode_t mode;
    uint32_t master_pct;  // default 62%
    uint32_t gap_inner;   // default 12px
    uint32_t gap_outer_x; // left: 80px (dock), right: 16px
    uint32_t gap_outer_y; // top: 60px (island), bottom: 16px
} layout_engine_t;

void layout_init(void);
void layout_apply(window_t *window_list, uint8_t active_workspace, uint32_t screen_w, uint32_t screen_h);
void layout_cycle_mode(void);
const char *layout_get_mode_name(void);

#endif
