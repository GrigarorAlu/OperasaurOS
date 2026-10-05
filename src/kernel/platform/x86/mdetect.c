#include "../mdetect.h"

#include "../../tty.h"

typedef struct
{
    u32 addr;
    u32 addr_high;
    u32 len;
    u32 len_high;
    u32 type;
    u32 acpi_flags;
} MemRegion;

typedef enum
{
    REGION_NULL,
    REGION_FREE,
    REGION_RESERVED,
    REGION_ACPI_RECLAIMABLE,
    REGION_ACPI_RESERVED,
    REGION_BAD,
} RegionType;

// Created in bootloader
static u16 *mem_region_count = (void *)0x7E00;
static MemRegion *mem_regions = (void *)0x7E08;

void mdetect_fill(u32 *bitmap, usz bitmap_size, usz page_size)
{
    printf(&tty, TTY_DEBUG "Found %u memory regions:\n", *mem_region_count);
    for (u16 i = 0; i < *mem_region_count; i++)
        printf(&tty, "\taddr=%8x, len=%8x, type=%8x, flags=%8x\n", mem_regions[i].addr, mem_regions[i].len,
               mem_regions[i].type, mem_regions[i].acpi_flags);
    put(&tty, '\n');

    for (u16 i = 0; i < *mem_region_count; i++)
        if (mem_regions[i].type == REGION_FREE)
        {
            usz addr = mem_regions[i].addr;
            usz len = mem_regions[i].len;

            usz start = addr == 0 ? 0 : (addr - 1) / page_size + 1;
            usz end = (addr + len) / page_size;

            for (u8 j = start % BITNESS; j < BITNESS; j++) bitmap[start / BITNESS] |= 1 << j;
            for (usz j = start / BITNESS + 1; j < end / BITNESS; j++) bitmap[j] = ~0;
            for (u8 j = 0; j < end % BITNESS; j++) bitmap[end / BITNESS] |= 1 << j;
        }

    for (u16 i = 0; i < *mem_region_count; i++)
        if (mem_regions[i].type != REGION_FREE)
        {
            usz addr = mem_regions[i].addr;
            usz len = mem_regions[i].len;

            usz start = addr / page_size;
            usz end = (addr + len - 1) / page_size + 1;

            for (u8 j = start % BITNESS; j < BITNESS; j++) bitmap[start / BITNESS] &= ~(1 << j);
            for (usz j = start / BITNESS + 1; j < end / BITNESS; j++) bitmap[j] = 0;
            for (u8 j = 0; j < end % BITNESS; j++) bitmap[end / BITNESS] &= ~(1 << j);
        }
}
