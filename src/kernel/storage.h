#ifndef STORAGE_H
#define STORAGE_H

#include <stream.h>
#include <types.h>

typedef struct
{
    Stream stream;
    u32 block_count;
    u32 block_size;
    u32 pos;
} Storage;

// WARNING: reading and writing expects block count instead of buffer size in bytes!
Storage *storage(i8 id);

i8 storage_add(Storage *storage);

void storage_remove(i8 id);

#endif
