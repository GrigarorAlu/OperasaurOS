#ifndef IO_H
#define IO_H

#include <types.h>

static inline void io_outb(u16 port, u8 value)
{
    __asm__ volatile("outb %b0, %w1" : : "a"(value), "Nd"(port));
}

static inline u8 io_inb(u16 port)
{
    u8 value;
    __asm__ volatile("inb %w1, %b0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void io_outw(u16 port, u16 value)
{
    __asm__ volatile("outw %w0, %w1" : : "a"(value), "Nd"(port));
}

static inline u16 io_inw(u16 port)
{
    u16 value;
    __asm__ volatile("inw %w1, %w0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void io_outl(u16 port, u32 value)
{
    __asm__ volatile("outl %l0, %w1" : : "a"(value), "Nd"(port));
}

static inline u32 io_inl(u16 port)
{
    u32 value;
    __asm__ volatile("inl %w1, %l0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void io_outsb(u16 port, const u8 *addr, usz count)
{
    __asm__ volatile("rep outsb" : "+S"(addr), "+c"(count) : "d"(port) : "memory");
}

static inline void io_insb(u16 port, u8 *addr, usz count)
{
    __asm__ volatile("rep insb" : "+D"(addr), "+c"(count) : "d"(port) : "memory");
}

static inline void io_outsw(u16 port, const u16 *addr, usz count)
{
    __asm__ volatile("rep outsw" : "+S"(addr), "+c"(count) : "d"(port) : "memory");
}

static inline void io_insw(u16 port, u16 *addr, usz count)
{
    __asm__ volatile("rep insw" : "+D"(addr), "+c"(count) : "d"(port) : "memory");
}

static inline void io_outsl(u16 port, const u32 *addr, usz count)
{
    __asm__ volatile("rep outsl" : "+S"(addr), "+c"(count) : "d"(port) : "memory");
}

static inline void io_insl(u16 port, u32 *addr, usz count)
{
    __asm__ volatile("rep insl" : "+D"(addr), "+c"(count) : "d"(port) : "memory");
}
static inline void io_wait()
{
    io_outb(0x80, 0);
}

#endif
