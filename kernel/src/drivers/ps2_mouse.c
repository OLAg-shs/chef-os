#include "drivers/ps2_mouse.h"
#include "arch/io.h"
#include "arch/idt.h"

static mouse_state_t s_mouse_state = {400, 300, false, false, false};
static uint8_t s_mouse_cycle = 0;
static uint8_t s_mouse_bytes[3];
static uint32_t s_max_x = 1280;
static uint32_t s_max_y = 800;

static void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(0x64) & 1) == 1) return;
        }
    } else {
        while (timeout--) {
            if ((inb(0x64) & 2) == 0) return;
        }
    }
}

static void mouse_write(uint8_t write) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, write);
}

static uint8_t mouse_read(void) {
    mouse_wait(0);
    return inb(0x60);
}

static void mouse_callback(registers_t *regs) {
    (void)regs;
    uint8_t status = inb(0x64);
    if (!(status & 0x20)) return; // Not mouse data

    uint8_t byte = inb(0x60);
    s_mouse_bytes[s_mouse_cycle++] = byte;

    if (s_mouse_cycle == 3) {
        s_mouse_cycle = 0;

        uint8_t flags = s_mouse_bytes[0];
        int32_t dx = (int32_t)s_mouse_bytes[1];
        int32_t dy = (int32_t)s_mouse_bytes[2];

        if (flags & 0x10) dx |= 0xFFFFFF00;
        if (flags & 0x20) dy |= 0xFFFFFF00;

        s_mouse_state.left_button   = (flags & 0x01) != 0;
        s_mouse_state.right_button  = (flags & 0x02) != 0;
        s_mouse_state.middle_button = (flags & 0x04) != 0;

        s_mouse_state.x += dx;
        s_mouse_state.y -= dy; // Invert Y delta

        if (s_mouse_state.x < 0) s_mouse_state.x = 0;
        if (s_mouse_state.y < 0) s_mouse_state.y = 0;
        if (s_mouse_state.x >= (int32_t)s_max_x) s_mouse_state.x = s_max_x - 1;
        if (s_mouse_state.y >= (int32_t)s_max_y) s_mouse_state.y = s_max_y - 1;
    }
}

void ps2_mouse_init(uint32_t screen_width, uint32_t screen_height) {
    s_max_x = screen_width;
    s_max_y = screen_height;
    s_mouse_state.x = screen_width / 2;
    s_mouse_state.y = screen_height / 2;

    // Enable Auxiliary Device (Mouse)
    mouse_wait(1);
    outb(0x64, 0xA8);

    // Enable Interrupts in Compak controller
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    uint8_t status = inb(0x60) | 2;
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);

    // Set Default Settings
    mouse_write(0xF6);
    mouse_read();

    // Enable Packet Streaming
    mouse_write(0xF4);
    mouse_read();

    register_interrupt_handler(44, mouse_callback); // IRQ 12 = Interrupt 44
}

void ps2_mouse_get_state(mouse_state_t *state) {
    if (state) *state = s_mouse_state;
}
