#include "drivers/rtc.h"
#include "arch/io.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

static uint8_t get_rtc_register(int reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static uint8_t bcd_to_bin(uint8_t bcd) {
    return ((bcd / 16) * 10) + (bcd & 0x0F);
}

void rtc_init(void) {
    // CMOS RTC ready
}

void rtc_get_time(rtc_time_t *time) {
    if (!time) return;

    time->second = bcd_to_bin(get_rtc_register(0x00));
    time->minute = bcd_to_bin(get_rtc_register(0x02));
    time->hour   = bcd_to_bin(get_rtc_register(0x04));
    time->day    = bcd_to_bin(get_rtc_register(0x07));
    time->month  = bcd_to_bin(get_rtc_register(0x08));
    time->year   = 2000 + bcd_to_bin(get_rtc_register(0x09));
}
