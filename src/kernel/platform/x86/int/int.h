#ifndef INT_H
#define INT_H

#include <types.h>

typedef struct
{
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, _;
    u32 ebx, edx, ecx, eax;
    u32 int_num, err;
    u32 eip, cs, eflags, esp, ss;
} RegState;

typedef void (*IntHandlerFn)(RegState *state);

void int_init();

void int_register(u8 num, IntHandlerFn handler);

#endif
