#ifndef PAGE_H
#define PAGE_H

#define PAGE_SIZE 4096

#include <types.h>

typedef enum
{
    PAGE_PRESENT = 0x01,
    PAGE_WRITE = 0x02,
    PAGE_USER = 0x04,
    PAGE_WRITETHROUGH = 0x08,
    PAGE_NOCACHE = 0x10,
    PAGE_ACCESSED = 0x20,
    PAGE_TABLE_DIRTY = 0x40,
    PAGE_TABLE_PAT = 0x80,
} PageEntryFlags;

void page_init();

void *page_map(void *physical, void *virtual, u8 flags);

void page_unmap(void *virtual);

#endif
