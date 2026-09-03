#ifndef CHEF_PIC_H
#define CHEF_PIC_H

#include "types.h"

void pic_remap(int offset1, int offset2);
void pic_disable(void);
void pic_send_eoi(uint8_t irq);

#endif
