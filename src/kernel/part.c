#include "part.h"

#include "fs/fs.h"
#include "storage.h"

#define MAX_PARTITION_COUNT 26
Partition partitions[MAX_PARTITION_COUNT];

typedef struct __attribute__((packed))
{
    u8 bootable;
    u8 start_head;
    u16 start_cylinder_sector;
    u8 type;
    u8 end_head;
    u16 end_cylinder_sector;
    u32 lba;
    u32 sector_count;
} MbrEntry;

typedef struct __attribute__((packed))
{
    u8 bootstrap[446];
    MbrEntry entries[4];
    u16 signature;
} Mbr;

isz partition_read(const Partition *partition, u32 lba, void *buffer, usz count)
{
    Storage *dev = storage(partition->storage_id);
    dev->pos = partition->lba + lba;
    return read(dev, buffer, count);
}

isz partition_write(const Partition *partition, u32 lba, const void *buffer, usz count)
{
    Storage *dev = storage(partition->storage_id);
    dev->pos = partition->lba + lba;
    return write(dev, buffer, count);
}

void partitions_add(i8 storage_id)
{
    Storage *dev = storage(storage_id);
    Mbr mbr;
    read(dev, &mbr, 1);

    if (mbr.signature != 0xAA55)
    {
        Partition partition = {.lba = 0, .size = dev->block_count, .storage_id = storage_id};
        // if (fs_identify(&partition))
        for (u8 i = 0; i < MAX_PARTITION_COUNT; i++)
            if (partitions[i].fs_type != 0)
            {
                partitions[i] = partition;
                return;
            }
        return;
    }

    u8 partition_id = 0;
    for (u8 i = 0; i < 4; i++)
    {
        MbrEntry *entry = &mbr.entries[i];
        if (entry->type > 0)
        {
            Partition partition = {.lba = entry->lba, .size = entry->sector_count, .storage_id = storage_id};
            // if (fs_identify(&partition))
            {
                for (; partition_id < MAX_PARTITION_COUNT; partition_id++)
                    if (partitions[partition_id].fs_type != 0)
                    {
                        partitions[partition_id] = partition;
                        break;
                    }
                if (partition_id == MAX_PARTITION_COUNT) return;
            }
        }
    }
}

void partitions_remove(i8 storage_id)
{
    for (u8 i = 0; i < MAX_PARTITION_COUNT; i++)
        if (partitions[i].storage_id == storage_id) partitions[i] = (Partition){};
}

Partition *partition(u8 id)
{
    return &partitions[id];
}
