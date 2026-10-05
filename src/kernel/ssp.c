#include "panic.h"
#include <types.h>

usz __stack_chk_guard = 0x58F27E1D;

__attribute__((noreturn)) void __stack_chk_fail()
{
    panic("Stack smashed!");
}
