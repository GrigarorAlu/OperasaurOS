#include "platform/platform.h"
#include "tty.h"

__attribute__((section(".text.main"))) __attribute__((noreturn)) void main()
{
    platform_init();

    puts(&tty, TTY_CUSTOM("\x1D") "Hello from kernel!\n");

    while (true);
}
