#ifndef PANIC_H
#define PANIC_H

#include <stream.h>
#include <types.h>

__attribute__((noreturn)) void panic(const u8 *msg);

#endif
