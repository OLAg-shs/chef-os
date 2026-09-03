#include "compositor/layout.h"

static layout_engine_t s_layout;

void layout_init(void) {
    s_layout.mode = LAYOUT_MASTER_STACK;
    s_layout.master_pct = 62;
    s_layout.gap_inner = 12;
    s_layout.gap_outer_x = 84;
    s_layout.gap_outer_y = 60;
}

void layout_cycle_mode(void) {
    s_layout.mode = (s_layout.mode + 1) % 4;
}

const char *layout_get_mode_name(void) {
    switch (s_layout.mode) {
        case LAYOUT_MASTER_STACK: return "Master-Stack";
        case LAYOUT_GRID:         return "Grid";
        case LAYOUT_MONOCLE:      return "Monocle";
        case LAYOUT_FLOATING:     return "Floating";
        default:                  return "Default";
    }
}

void layout_apply(window_t *window_list, uint8_t active_workspace, uint32_t screen_w, uint32_t screen_h) {
    // Count visible windows on current workspace
    int count = 0;
    window_t *visible[16];

    window_t *w = window_list;
    while (w && count < 16) {
        if (w->workspace_id == active_workspace && !w->is_minimized) {
            visible[count++] = w;
        }
        w = w->next;
    }

    if (count == 0) return;

    uint32_t start_x = s_layout.gap_outer_x;
    uint32_t start_y = s_layout.gap_outer_y;
    uint32_t avail_w = screen_w - start_x - 16;
    uint32_t avail_h = screen_h - start_y - 16;
    uint32_t gap = s_layout.gap_inner;

    if (s_layout.mode == LAYOUT_MONOCLE || count == 1) {
        for (int i = 0; i < count; i++) {
            visible[i]->x = start_x;
            visible[i]->y = start_y;
            visible[i]->width = avail_w;
            visible[i]->height = avail_h;
            window_resize_surface(visible[i], avail_w, avail_h);
        }
    } else if (s_layout.mode == LAYOUT_MASTER_STACK) {
        uint32_t master_w = ((avail_w * s_layout.master_pct) / 100) - (gap / 2);
        uint32_t stack_w  = avail_w - master_w - gap;

        // Master window (first)
        visible[0]->x = start_x;
        visible[0]->y = start_y;
        visible[0]->width = master_w;
        visible[0]->height = avail_h;
        window_resize_surface(visible[0], master_w, avail_h);

        // Stack windows (remaining)
        int stack_count = count - 1;
        uint32_t stack_h = (avail_h - (gap * (stack_count - 1))) / stack_count;
        uint32_t curr_y = start_y;

        for (int i = 1; i < count; i++) {
            visible[i]->x = start_x + master_w + gap;
            visible[i]->y = curr_y;
            visible[i]->width = stack_w;
            visible[i]->height = (i == count - 1) ? (start_y + avail_h - curr_y) : stack_h;
            window_resize_surface(visible[i], visible[i]->width, visible[i]->height);
            curr_y += stack_h + gap;
        }
    } else if (s_layout.mode == LAYOUT_GRID) {
        int cols = (count > 2) ? 2 : count;
        int rows = (count + cols - 1) / cols;

        uint32_t cell_w = (avail_w - (gap * (cols - 1))) / cols;
        uint32_t cell_h = (avail_h - (gap * (rows - 1))) / rows;

        for (int i = 0; i < count; i++) {
            int c = i % cols;
            int r = i / cols;
            visible[i]->x = start_x + c * (cell_w + gap);
            visible[i]->y = start_y + r * (cell_h + gap);
            visible[i]->width = cell_w;
            visible[i]->height = cell_h;
            window_resize_surface(visible[i], cell_w, cell_h);
        }
    }
}
