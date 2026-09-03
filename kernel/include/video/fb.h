#ifndef CHEF_FB_H
#define CHEF_FB_H

#include "types.h"
#include "limine.h"

/* Chef Cream Design Tokens */
#define COLOR_BG          0x00F1EBDD  /* Warm Cream Background */
#define COLOR_SURFACE     0x00F8F4EA  /* Frosted Surface */
#define COLOR_SURFACE_ALT 0x00E8E0CE  /* Surface Darker */
#define COLOR_TEXT        0x002B2A28  /* Dark Ink Typography */
#define COLOR_TEXT_MUTED  0x006E6A5F  /* Muted Slate/Ink */
#define COLOR_ACCENT      0x00A6534A  /* Restrained Red */
#define COLOR_BORDER      0x00DCD3BE  /* Border Parchment */
#define COLOR_WHITE       0x00FFFFFF
#define COLOR_GREEN       0x004A7C59
#define COLOR_BLUE        0x003D6B8C

typedef struct {
    uint32_t *frontbuffer;
    uint32_t *backbuffer;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint32_t bpp;
    uint32_t cursor_x;
    uint32_t cursor_y;
    uint32_t fg_color;
    uint32_t bg_color;
} fb_info_t;

void fb_init(struct limine_framebuffer *fb);
void fb_clear(uint32_t color);
void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void fb_draw_rounded_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t radius, uint32_t color, uint32_t border_color);
void fb_draw_char(char c, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg);
void fb_draw_string(const char *str, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg);
void fb_putc(char c);
void fb_puts(const char *str);
void fb_swap(void);
void fb_draw_desktop_shell(void);

#endif
