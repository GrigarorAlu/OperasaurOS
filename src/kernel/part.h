#ifndef PART_H
#define PART_H

#include <stream.h>
#include <types.h>

typedef struct
{
    u32 lba;
    u32 size;
    u16 _;
    i8 storage_id;
    u8 fs_type;
    u8 fs_data[28];
} Partition;

isz partition_read(const Partition *partition, u32 lba, void *buffer, usz count);

isz partition_write(const Partition *partition, u32 lba, const void *buffer, usz count);

void partitions_add(i8 storage_id);

void partitions_remove(i8 storage_id);

Partition *partition(u8 id);

#define partition_id(letter) ((letter) - 'A')

#endif
