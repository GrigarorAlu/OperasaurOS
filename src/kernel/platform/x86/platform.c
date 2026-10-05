#include "../../keyboard.h"
#include "../../panic.h"
#include "../../phalloc.h"
#include "../../tty.h"
#include "ata.h"
#include "int/int.h"
#include "int/irq.h"
#include "int/pic.h"
#include "ps2.h"

static u32 ticks = 0;

void timer_handler(RegState *regs)
{
    // stream_printf(tty_stream(), "Time = %u.\r", ticks);
    ticks++;
}

void keycode_handler(KeyCode key, bool pressed)
{
    // stream_printf(tty_stream(), "%u", key);
}

void keychar_handler(u8 c)
{
    put(&tty, c);
}

void platform_init()
{
    tty_init();
    int_init();
    pic_init();
    irq_init();

    phalloc_init();

    u8 ps2_features = ps2_init();
    if (ps2_features & PS2_KEYBOARD) keyboard_init();
    else panic("There's no PS/2 keyboard :(\n");

    key_code_handler = &keycode_handler;
    key_char_handler = &keychar_handler;

    irq_register(0, &timer_handler);

    __asm__ volatile("sti");

    ata_init();
}

void platform_halt()
{
    __asm__ volatile("cli; hlt");
}
