#include "fs.h"

#include "../storage.h"

typedef struct __attribute__((packed))
{
    u8 jump[3];
    u8 oem_id[8];
    u16 sector_size;
    u8 cluster_size;
    u16 reserved_sectors;
    u8 fat_count;
    u16 root_size;
    u16 sector_count;
    u8 media_type;
    u16 fat_size;
    u16 track_size;
    u16 head_count;
    u32 partition_start;
    u32 large_sector_count;
} Bpb;

typedef struct __attribute__((packed))
{
    u8 drive_num;
    u8 nt_flags;
    u8 signature;
    u8 volume_id[4];
    u8 label[11];
    u8 system_id[8];
} OldEbr;

typedef struct __attribute__((packed))
{
    u32 fat_size;
    u16 flags;
    u16 version;
    u32 root_cluster;
    u16 info_sector;
    u16 backup_sector;
    u8 _[12];
} NewEbr;

typedef union
{
    u8 bytes[512];
    struct
    {
        Bpb bpb;
        union
        {
            OldEbr ebr;
            struct
            {

                NewEbr ebr1;
                OldEbr ebr2;
            };
        };
    };
} BootSector;

typedef struct
{
    u32 lead_signature;
    u8 _[480];
    u32 middle_signature;
    u32 free_cluster_count;
    u32 last_alloc_cluster;
    u8 __[12];
    u32 trail_signature;
} FsInfo;

typedef struct
{
    u32 free_cluster_count;
    u32 last_alloc_cluster;
    u32 data_start;
    u32 fat_size;
    u32 root_cluster;
    u32 root_start;
    u16 info_sector;
    u16 fat_start;
    u16 root_size;
    u8 fat_count;
    u8 cluster_size;
    u8 fat_type;
} FsData;

typedef enum
{
    NONE,
    FAT12,
    FAT16,
    FAT32,
} FatType;

static bool identify(Partition *partition)
{
    FsData *data = (FsData *)partition->fs_data;
    Storage *dev = storage(partition->storage_id);

    BootSector bs;
    partition_read(partition, 0, &bs, 1);

    if (bs.bpb.sector_size != dev->block_size) return false;
    if (bs.bpb.partition_start != partition->lba) return false;

    u32 sector_count = bs.bpb.sector_count == 0 ? bs.bpb.large_sector_count : bs.bpb.sector_count;
    if (sector_count != partition->size) return false;

    data->fat_type = sector_count > 65524 ? FAT32 : (sector_count > 4084 ? FAT16 : FAT12);
    data->cluster_size = bs.bpb.cluster_size;
    data->root_size = bs.bpb.root_size;
    data->fat_count = bs.bpb.fat_count;
    data->fat_start = bs.bpb.reserved_sectors;
    if (data->fat_type == FAT32)
    {
        data->fat_size = bs.ebr1.fat_size;
        data->root_cluster = bs.ebr1.root_cluster * data->cluster_size;
        data->info_sector = bs.ebr1.info_sector;

        FsInfo info;
        partition_read(partition, data->info_sector, &info, 1);

        if (info.lead_signature != 0x41615252) return false;
        if (info.middle_signature != 0x61417272) return false;
        if (info.trail_signature != 0xAA550000) return false;

        data->free_cluster_count = info.free_cluster_count;
        data->last_alloc_cluster = info.last_alloc_cluster;
    }
    else
    {
        data->fat_size = bs.bpb.fat_size;
        data->free_cluster_count = 0xFFFFFFFF;
        data->last_alloc_cluster = 0xFFFFFFFF;
    }
    data->root_start = data->fat_start + data->fat_count * data->fat_size;
    data->data_start = data->fat_start + data->root_size;

    partition->fs_type = FS_FAT;
    return true;
}

static File *open(const Partition *partition, const u8 *path) {}

static void close(const Partition *partition, File *file) {}

const FsInterface fat = {.identify = &identify, .open = &open, .close = &close};
