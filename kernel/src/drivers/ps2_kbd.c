#include "drivers/keyboard.h"
#include "arch/io.h"
#include "arch/idt.h"
#include "kprintf.h"

static volatile char s_last_char = 0;

static const char kbd_us[128] = {
    0, 27, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61, 8,
    9, 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93, 10,
    0, 97, 115, 100, 102, 103, 104, 106, 107, 108, 59, 39, 96,
    0, 92, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, 0,
    42, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void keyboard_callback(registers_t *regs) {
    (void)regs;
    uint8_t scancode = inb(0x60);
    if (!(scancode & 0x80)) {
        char c = kbd_us[scancode];
        if (c) {
            s_last_char = c;
            kprintf("%c", c);
        }
    }
}

void keyboard_init(void) {
    register_interrupt_handler(33, keyboard_callback);
}

char keyboard_get_last_char(void) {
    char c = s_last_char;
    s_last_char = 0;
    return c;
}
