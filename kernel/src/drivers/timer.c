#include "drivers/timer.h"
#include "arch/io.h"
#include "arch/idt.h"

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIT_BASE_HZ  1193180

static volatile uint64_t s_ticks = 0;

static void timer_callback(registers_t *regs) {
    (void)regs;
    s_ticks++;
}

void timer_init(uint32_t frequency_hz) {
    register_interrupt_handler(32, timer_callback);

    uint16_t divisor = (uint16_t)(PIT_BASE_HZ / frequency_hz);
    outb(PIT_COMMAND, 0x36); // Channel 0, lobyte/hibyte, rate generator
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
}

uint64_t timer_get_ticks(void) {
    return s_ticks;
}

void timer_sleep_ms(uint64_t ms) {
    uint64_t start = s_ticks;
    while (s_ticks - start < ms) {
        hlt();
    }
}
