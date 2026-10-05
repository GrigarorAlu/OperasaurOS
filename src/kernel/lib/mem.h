#ifndef _MEM_H
#define _MEM_H

#include <types.h>

void mem_copy(void *restrict dst, const void *restrict src, usz count);

void mem_move(void *dst, const void *src, usz count);

void mem_set(void *dst, usz count, u8 value);

bool mem_cmp(const void *a, const void *b, usz count);

bool str_cmp(const u8 *a, const u8 *b);

usz str_len(const u8 *str);

#define copy(dst, src, count) mem_copy(dst, src, (count) * sizeof(typeof(src)))
#define move(dst, src, count) mem_move(dst, src, (count) * sizeof(typeof(src)))
#define cmp(dst, src, count) mem_cmp(dst, src, (count) * sizeof(typeof(src)))

#endif
