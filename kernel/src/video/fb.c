#include "video/fb.h"
#include "string.h"

extern const uint8_t font_8x16[128][16];

static fb_info_t g_fb;
static uint32_t s_backbuffer[1920 * 1080];

void fb_init(struct limine_framebuffer *fb) {
    if (!fb) return;
    g_fb.frontbuffer = (uint32_t *)fb->address;
    g_fb.backbuffer = s_backbuffer;
    g_fb.width = fb->width;
    g_fb.height = fb->height;
    g_fb.pitch = fb->pitch / 4;
    g_fb.bpp = fb->bpp;
    g_fb.cursor_x = 80;
    g_fb.cursor_y = 60;
    g_fb.fg_color = COLOR_TEXT;
    g_fb.bg_color = COLOR_BG;

    fb_clear(COLOR_BG);
    fb_swap();
}

void fb_clear(uint32_t color) {
    for (uint64_t y = 0; y < g_fb.height; y++) {
        for (uint64_t x = 0; x < g_fb.width; x++) {
            g_fb.backbuffer[y * g_fb.width + x] = color;
        }
    }
}

void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= g_fb.width || y >= g_fb.height) return;
    g_fb.backbuffer[y * g_fb.width + x] = color;
}

void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t j = y; j < y + h && j < g_fb.height; j++) {
        for (uint32_t i = x; i < x + w && i < g_fb.width; i++) {
            g_fb.backbuffer[j * g_fb.width + i] = color;
        }
    }
}

void fb_draw_rounded_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t radius, uint32_t color, uint32_t border_color) {
    (void)radius;
    fb_draw_rect(x, y, w, h, color);
    if (border_color != 0) {
        for (uint32_t i = x; i < x + w; i++) {
            fb_put_pixel(i, y, border_color);
            fb_put_pixel(i, y + h - 1, border_color);
        }
        for (uint32_t j = y; j < y + h; j++) {
            fb_put_pixel(x, j, border_color);
            fb_put_pixel(x + w - 1, j, border_color);
        }
    }
}

void fb_draw_char(char c, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg) {
    if ((uint8_t)c > 127) c = '?';
    const uint8_t *glyph = font_8x16[(uint8_t)c];
    for (int row = 0; row < 16; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if ((bits >> (7 - col)) & 1) {
                fb_put_pixel(x + col, y + row, fg);
            } else if (bg != 0) {
                fb_put_pixel(x + col, y + row, bg);
            }
        }
    }
}

void fb_draw_string(const char *str, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg) {
    while (*str) {
        fb_draw_char(*str, x, y, fg, bg);
        x += 8;
        str++;
    }
}

void fb_putc(char c) {
    if (c == '\n') {
        g_fb.cursor_x = 88 + 16;
        g_fb.cursor_y += 18;
        return;
    }
    if (g_fb.cursor_x + 8 >= g_fb.width - 40) {
        g_fb.cursor_x = 88 + 16;
        g_fb.cursor_y += 18;
    }
    fb_draw_char(c, g_fb.cursor_x, g_fb.cursor_y, g_fb.fg_color, 0);
    g_fb.cursor_x += 8;
}

void fb_puts(const char *str) {
    while (*str) {
        fb_putc(*str++);
    }
}

void fb_swap(void) {
    if (!g_fb.frontbuffer || !g_fb.backbuffer) return;
    for (uint64_t y = 0; y < g_fb.height; y++) {
        memcpy(&g_fb.frontbuffer[y * g_fb.pitch], &g_fb.backbuffer[y * g_fb.width], g_fb.width * 4);
    }
}

void fb_draw_desktop_shell(void) {
    fb_clear(COLOR_BG);

    uint32_t dock_x = 12;
    uint32_t dock_y = 12;
    uint32_t dock_w = 56;
    uint32_t dock_h = g_fb.height - 24;

    fb_draw_rounded_rect(dock_x, dock_y, dock_w, dock_h, 16, COLOR_SURFACE, COLOR_BORDER);

    fb_draw_rounded_rect(dock_x + 10, dock_y + 12, 36, 36, 8, COLOR_ACCENT, 0);
    fb_draw_string("C", dock_x + 24, dock_y + 22, COLOR_WHITE, 0);

    uint32_t ws_y = dock_y + 60;
    const char *ws_labels[] = {"1", "2", "3", "4", "+"};
    for (int i = 0; i < 5; i++) {
        uint32_t pill_bg = (i == 0) ? COLOR_ACCENT : COLOR_SURFACE_ALT;
        uint32_t pill_fg = (i == 0) ? COLOR_WHITE : COLOR_TEXT;
        fb_draw_rounded_rect(dock_x + 14, ws_y + (i * 38), 28, 28, 6, pill_bg, COLOR_BORDER);
        fb_draw_string(ws_labels[i], dock_x + 24, ws_y + (i * 38) + 6, pill_fg, 0);
    }

    uint32_t bot_y = dock_y + dock_h - 130;
    fb_draw_rounded_rect(dock_x + 14, bot_y, 28, 28, 6, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string("v", dock_x + 24, bot_y + 6, COLOR_TEXT_MUTED, 0);

    fb_draw_rounded_rect(dock_x + 14, bot_y + 36, 28, 28, 6, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string("w", dock_x + 24, bot_y + 42, COLOR_TEXT_MUTED, 0);

    fb_draw_rounded_rect(dock_x + 14, bot_y + 72, 28, 28, 6, COLOR_ACCENT, 0);
    fb_draw_string("x", dock_x + 24, bot_y + 78, COLOR_WHITE, 0);

    uint32_t island_w = 260;
    uint32_t island_h = 36;
    uint32_t island_x = (g_fb.width - island_w) / 2;
    uint32_t island_y = 12;

    fb_draw_rounded_rect(island_x, island_y, island_w, island_h, 18, COLOR_SURFACE, COLOR_BORDER);
    fb_draw_string("Chef OS  ::  Ready", island_x + 48, island_y + 10, COLOR_TEXT, 0);

    uint32_t win_x = 88;
    uint32_t win_y = 60;
    uint32_t win_w = g_fb.width - 108;
    uint32_t win_h = g_fb.height - 80;

    fb_draw_rounded_rect(win_x, win_y, win_w, win_h, 12, COLOR_SURFACE, COLOR_BORDER);
    fb_draw_rounded_rect(win_x, win_y, win_w, 30, 12, COLOR_SURFACE_ALT, COLOR_BORDER);
    fb_draw_string("Chef OS Native Kernel Console (v1.0.0-alpha)  [Super+W to close]", win_x + 16, win_y + 8, COLOR_TEXT, 0);

    g_fb.cursor_x = win_x + 16;
    g_fb.cursor_y = win_y + 42;

    fb_swap();
}

fb_info_t *fb_get_info(void) {
    return &g_fb;
}

void fb_draw_cursor(int32_t x, int32_t y) {
    // 10x15 Arrow Mouse Cursor in Restrained Accent / Dark Ink
    static const uint8_t cursor_bitmap[15][10] = {
        {1,0,0,0,0,0,0,0,0,0},
        {1,1,0,0,0,0,0,0,0,0},
        {1,2,1,0,0,0,0,0,0,0},
        {1,2,2,1,0,0,0,0,0,0},
        {1,2,2,2,1,0,0,0,0,0},
        {1,2,2,2,2,1,0,0,0,0},
        {1,2,2,2,2,2,1,0,0,0},
        {1,2,2,2,2,2,2,1,0,0},
        {1,2,2,2,2,1,1,1,1,0},
        {1,2,2,1,2,1,0,0,0,0},
        {1,2,1,0,1,2,1,0,0,0},
        {1,1,0,0,1,2,1,0,0,0},
        {1,0,0,0,0,1,2,1,0,0},
        {0,0,0,0,0,1,2,1,0,0},
        {0,0,0,0,0,0,1,1,0,0}
    };

    for (int r = 0; r < 15; r++) {
        for (int c = 0; c < 10; c++) {
            uint8_t val = cursor_bitmap[r][c];
            if (val == 1) {
                fb_put_pixel(x + c, y + r, COLOR_TEXT);
            } else if (val == 2) {
                fb_put_pixel(x + c, y + r, COLOR_WHITE);
            }
        }
    }
}
