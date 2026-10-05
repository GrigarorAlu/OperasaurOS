#ifndef VFS_H
#define VFS_H

#include <types.h>

#include "../storage.h"

typedef struct
{
    Storage *storage;
    u32 pos;
    u32 size;
    u8 name[8];
} VfsMountpoint;

typedef struct
{
    u32 pos;
    u32 size;
    u8 name[15];
} VfsEntry;

void vfs_init();

void vfs_add_storage(Storage *storage);

void vfs_remove_storage(const u8 *id);

#endif
