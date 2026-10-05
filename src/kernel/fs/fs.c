#include "fs.h"

#include "../part.h"

extern const FsInterface fat;

const FsInterface *filesystems[] = {&fat};

bool fs_identify(Partition *partition)
{
    for (u8 i = 0; i < countof(filesystems); i++)
        if (filesystems[i]->identify(partition)) return true;
    return false;
}
