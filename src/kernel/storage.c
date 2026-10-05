#include "storage.h"

#include "part.h"
#include <mem.h>

#define MAX_STORAGE_COUNT 4
static Storage *storages[MAX_STORAGE_COUNT] = {};

Storage *storage(i8 id)
{
    return storages[(u8)id];
}

i8 storage_add(Storage *storage)
{
    if (!storage->stream.read || !storage->stream.write) return -1;

    for (u8 i = 0; i < MAX_STORAGE_COUNT; i++)
        if (storages[i] == null)
        {
            storages[i] = storage;
            partitions_add(i);
            return i;
        }
    return -1;
}

void storage_remove(i8 id)
{
    partitions_remove(id);
    storages[(u8)id] = null;
}
