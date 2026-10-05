#include "mem.h"

void mem_copy(void *restrict dst, const void *restrict src, usz count)
{
    for (usz i = 0; i < count; i++) ((u8 *)dst)[i] = ((u8 *)src)[i];
}

void mem_move(void *dst, const void *src, usz count)
{
    if (dst < src)
        for (usz i = 0; i < count; i++) ((u8 *)dst)[i] = ((u8 *)src)[i];
    else
        for (usz i = count - 1; i != (usz)-1; i--) ((u8 *)dst)[i] = ((u8 *)src)[i];
}

void mem_set(void *dst, usz count, u8 value)
{
    for (usz i = 0; i < count; i++) ((u8 *)dst)[i] = value;
}

bool mem_cmp(const void *a, const void *b, usz count)
{
    for (usz i = 0; i < count; i++)
        if (((u8 *)a)[i] != ((u8 *)b)[i]) return false;
    return true;
}

bool str_cmp(const u8 *a, const u8 *b)
{
    while (*a && *b)
        if (*a++ != *b++) return false;
    return true;
}

usz str_len(const u8 *str)
{
    usz len = 0;
    while (*str++) len++;
    return len;
}
