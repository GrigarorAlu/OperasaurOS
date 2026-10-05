#include "ata.h"

#include "io.h"
#include <mem.h>

#define BUS1 0x1F0
#define BUS2 0x170

#define PORT_DATA 0
#define PORT_ERROR 1
#define PORT_FEATURES 1
#define PORT_COUNT 2
#define PORT_LBA_LOW 3
#define PORT_LBA_MID 4
#define PORT_LBA_HIGH 5
#define PORT_SELECT 6
#define PORT_STATUS 7
#define PORT_CMD 7

#define CMD_IDENTIFY 0xEC
#define CMD_READ_PIO 0x20
#define CMD_WRITE_PIO 0x30
#define CMD_FLUSH 0xE7

typedef enum
{
    STATUS_ERR = 0x01,
    STATUS_INDEX = 0x02,
    STATUS_CORRECTED = 0x04,
    STATUS_DATA = 0x08,
    STATUS_SERVICE = 0x10,
    STATUS_FAULT = 0x20,
    STATUS_READY = 0x40,
    STATUS_BUSY = 0x80,
} Status;

typedef enum
{
    SELECT = 0xA0,
    SELECT_SLAVE = 0x10,
    SELECT_LBA = 0x40,
} Select;

static isz disk_read_pio(void *self, void *buffer, usz count)
{
    AtaDrive *drive = self;

    io_outb(drive->bus + PORT_SELECT, SELECT | SELECT_LBA | (drive->storage.pos >> 24 & 0xF));
    io_outb(drive->bus + PORT_COUNT, count);
    io_outb(drive->bus + PORT_LBA_LOW, (u8)drive->storage.pos);
    io_outb(drive->bus + PORT_LBA_MID, (u8)(drive->storage.pos >> 8));
    io_outb(drive->bus + PORT_LBA_HIGH, (u8)(drive->storage.pos >> 16));
    io_outb(drive->bus + PORT_CMD, CMD_READ_PIO);

    for (usz i = 0; i < count; i++)
    {
        for (u8 status = 0; !(status & STATUS_DATA); status = io_inb(drive->bus + PORT_STATUS))
            if (status & STATUS_ERR) return i;
        io_insw(drive->bus + PORT_DATA, &((u16 *)buffer)[i * 256], 256);
    }

    return count;
}

static isz disk_write_pio(void *self, const void *buffer, usz count)
{
    AtaDrive *drive = self;

    io_outb(drive->bus + PORT_SELECT, SELECT | SELECT_LBA | (drive->storage.pos >> 24 & 0xF));
    io_outb(drive->bus + PORT_COUNT, count);
    io_outb(drive->bus + PORT_LBA_LOW, (u8)drive->storage.pos);
    io_outb(drive->bus + PORT_LBA_MID, (u8)(drive->storage.pos >> 8));
    io_outb(drive->bus + PORT_LBA_HIGH, (u8)(drive->storage.pos >> 16));
    io_outb(drive->bus + PORT_CMD, CMD_WRITE_PIO);

    for (usz i = 0; i < count; i++)
    {
        for (u8 status = 0; !(status & STATUS_DATA); status = io_inb(drive->bus + PORT_STATUS))
            if (status & STATUS_ERR)
            {
                io_outb(drive->bus + PORT_CMD, CMD_FLUSH);
                return i;
            }
        for (u16 j = 0; j < 256; j++) io_outw(drive->bus + PORT_DATA, ((u16 *)buffer)[i * 256 + j]);
    }

    io_outb(drive->bus + PORT_CMD, CMD_FLUSH);
    return count;
}

AtaDrive disk1;
AtaDrive disk2;

static AtaDrive disk_identify(u16 bus)
{
    static u16 buffer[256];

    if (io_inb(bus + PORT_STATUS) == 0xFF) return (AtaDrive){};

    io_outb(bus + PORT_SELECT, SELECT);
    io_outb(bus + PORT_COUNT, 0);
    io_outb(bus + PORT_LBA_LOW, 0);
    io_outb(bus + PORT_LBA_MID, 0);
    io_outb(bus + PORT_LBA_HIGH, 0);
    io_outb(bus + PORT_CMD, CMD_IDENTIFY);
    if (io_inb(bus + PORT_STATUS) == 0) return (AtaDrive){};

    while (io_inb(bus + PORT_STATUS) & STATUS_BUSY)
        if (io_inb(bus + PORT_LBA_MID) != 0 || io_inb(bus + PORT_LBA_HIGH) != 0) return (AtaDrive){};

    for (u8 status = 0; !(status & STATUS_DATA); status = io_inb(bus + PORT_STATUS))
        if (status & STATUS_ERR) return (AtaDrive){};

    io_insw(bus + PORT_DATA, buffer, 256);

    return (AtaDrive){.storage = {.stream = {.read = &disk_read_pio, .write = &disk_write_pio},
                                  .block_count = buffer[60] | (u32)buffer[61] << 16,
                                  .block_size = 512},
                      .bus = bus};
}

void ata_init()
{
    disk1 = disk_identify(BUS1);
    storage_add(&disk1.storage);

    disk2 = disk_identify(BUS2);
    storage_add(&disk2.storage);
}
