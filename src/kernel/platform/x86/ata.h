#ifndef ATA_H
#define ATA_H

#include "../../storage.h"

typedef struct
{
    Storage storage;
    u16 bus;
} AtaDrive;

void ata_init();

#endif
