#include "kprintf.h"
#include "drivers/serial.h"
#include "video/fb.h"
#include "string.h"

static void print_num(uint64_t num, int base, int uppercase) {
    char buf[65];
    int i = 0;
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if (num == 0) {
        serial_putc('0');
        fb_putc('0');
        return;
    }

    while (num > 0) {
        buf[i++] = digits[num % base];
        num /= base;
    }

    while (--i >= 0) {
        serial_putc(buf[i]);
        fb_putc(buf[i]);
    }
}

void kvprintf(const char *fmt, va_list args) {
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'c': {
                    char c = (char)va_arg(args, int);
                    serial_putc(c);
                    fb_putc(c);
                    break;
                }
                case 's': {
                    const char *s = va_arg(args, const char *);
                    if (!s) s = "(null)";
                    serial_puts(s);
                    fb_puts(s);
                    break;
                }
                case 'd':
                case 'i': {
                    int64_t val = va_arg(args, int64_t);
                    if (val < 0) {
                        serial_putc('-');
                        fb_putc('-');
                        val = -val;
                    }
                    print_num((uint64_t)val, 10, 0);
                    break;
                }
                case 'u': {
                    uint64_t val = va_arg(args, uint64_t);
                    print_num(val, 10, 0);
                    break;
                }
                case 'x': {
                    uint64_t val = va_arg(args, uint64_t);
                    print_num(val, 16, 0);
                    break;
                }
                case 'X': {
                    uint64_t val = va_arg(args, uint64_t);
                    print_num(val, 16, 1);
                    break;
                }
                case 'p': {
                    uint64_t val = (uint64_t)va_arg(args, void *);
                    serial_puts("0x");
                    fb_puts("0x");
                    print_num(val, 16, 0);
                    break;
                }
                case '%': {
                    serial_putc('%');
                    fb_putc('%');
                    break;
                }
                default:
                    serial_putc('%');
                    serial_putc(*fmt);
                    fb_putc('%');
                    fb_putc(*fmt);
                    break;
            }
        } else {
            if (*fmt == '\n') {
                serial_putc('\r');
            }
            serial_putc(*fmt);
            fb_putc(*fmt);
        }
        fmt++;
    }
}

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    kvprintf(fmt, args);
    va_end(args);
    fb_swap();
}
