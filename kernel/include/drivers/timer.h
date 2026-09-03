#ifndef CHEF_TIMER_H
#define CHEF_TIMER_H

#include "types.h"

void timer_init(uint32_t frequency_hz);
uint64_t timer_get_ticks(void);
void timer_sleep_ms(uint64_t ms);

#endif
