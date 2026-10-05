#ifndef _TYPES_H
#define _TYPES_H

typedef char i8;
typedef short i16;
typedef long i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef i32 isz;
typedef u32 usz;

typedef u8 bool;
#define true 1
#define false 0

#define countof(arr) (sizeof(arr) / sizeof(arr[0]))
#define bitsizeof(T) (sizeof(T) * 8)
#define BITNESS bitsizeof(usz)

#define null (void *)0

#endif
