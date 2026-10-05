#ifndef PIC_H
#define PIC_H

#include <types.h>

void pic_init();

void pic_send_eoi(u8 irq);

#endif
