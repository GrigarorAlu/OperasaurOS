#ifndef _STREAM_H
#define _STREAM_H

#include "types.h"

typedef isz (*StreamReadFn)(void *self, void *buffer, usz size);
typedef isz (*StreamWriteFn)(void *self, const void *buffer, usz size);

typedef struct
{
    StreamReadFn read;
    StreamWriteFn write;
} Stream;

isz read(void *dev, void *buffer, usz size);
isz write(void *dev, const void *buffer, usz size);
isz get(void *dev, u8 *c);
isz put(void *dev, u8 c);
isz gets(void *dev, u8 *str);
isz puts(void *dev, const u8 *str);
isz printu(void *dev, u32 val, u8 width);
isz printi(void *dev, i32 val, u8 width);
isz printh(void *dev, u32 val, u8 width);
isz printf(void *dev, const u8 *format, ...);

#endif
