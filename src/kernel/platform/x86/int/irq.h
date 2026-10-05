#ifndef IRQ_H
#define IRQ_H

#include <types.h>

typedef void (*IrqHandlerFn)();

void irq_init();

void irq_register(u8 num, IrqHandlerFn handler);

#endif
