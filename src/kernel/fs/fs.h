#ifndef FS_H
#define FS_H

#include "../part.h"
#include <stream.h>

typedef struct
{
    Stream stream;
} File;

typedef bool (*FsIdentifyFn)(Partition *partition);
typedef File *(*FsOpenFn)(const Partition *partition, const u8 *path);
typedef void (*FsCloseFn)(const Partition *partition, File *file);

typedef struct
{
    FsIdentifyFn identify;
    FsOpenFn open;
    FsCloseFn close;
} FsInterface;

typedef enum
{
    FS_FAT,
} FsType;

bool fs_identify(Partition *partition);

#endif
