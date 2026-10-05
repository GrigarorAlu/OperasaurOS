#include "irq.h"

#include "../../../tty.h"
#include "int.h"
#include "pic.h"

#define REMAP_OFFSET 0x20

static IrqHandlerFn irq_handlers[16];

static void irq_handler(RegState *state)
{
    u8 irq = state->int_num - REMAP_OFFSET;

    if (irq_handlers[irq]) irq_handlers[irq]();
    else printf(&tty, "\x1BwUnhandled IRQ #%u.\n", irq);

    pic_send_eoi(irq);
}

void irq_init()
{
    for (u8 i = 0; i < 16; i++) int_register(REMAP_OFFSET + i, &irq_handler);
}

void irq_register(u8 num, IntHandlerFn handler)
{
    irq_handlers[num] = handler;
}
