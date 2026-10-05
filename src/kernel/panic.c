#include "panic.h"

#include "platform/platform.h"
#include "tty.h"

#include <mem.h>

__attribute__((noreturn)) void panic(const u8 *msg)
{
    tty.color = TTY_COLOR_CRIT;
    write(&tty, msg, str_len(msg));
    platform_halt();
}
