#include "phalloc.h"

#include "platform/mdetect.h"
#include "platform/page.h"
#include <mem.h>

#define BITMAP_SIZE (4LL * 1024 * 1024 * 1024 / PAGE_SIZE / BITNESS)
static usz bitmap[BITMAP_SIZE] = {};
static usz alloc_seek_start = 0;

void phalloc_init()
{
    mdetect_fill(bitmap, BITMAP_SIZE, PAGE_SIZE);
    page_init();
}

static void *phalloc_seek()
{
    for (u32 i = alloc_seek_start; i < BITMAP_SIZE; i++)
        for (u8 j = 0; j < BITNESS; j++)
            if (bitmap[i] >> j & 1)
            {
                usz page = i * BITNESS + j;
                alloc_seek_start = i;
                return (void *)(page * PAGE_SIZE);
            }
    return null;
}

void *phalloc_addr(void *addr)
{
    usz page = (usz)addr / PAGE_SIZE;
    if (!(bitmap[page / BITNESS] >> page % BITNESS & 1)) return null;
    bitmap[page / BITNESS] &= ~(1 << page % BITNESS);
    return page_map(addr, addr, PAGE_WRITE | PAGE_USER);
}

void *phalloc()
{
    void *addr = phalloc_seek();
    usz page = (usz)addr / PAGE_SIZE;
    bitmap[page / BITNESS] &= ~(1 << page % BITNESS);
    return page_map(addr, addr, PAGE_WRITE | PAGE_USER);
}

void phfree(void *addr)
{
    page_unmap(addr);
    usz page = (usz)addr / PAGE_SIZE;
    usz index = page / BITNESS;
    bitmap[index] |= 1 << page % BITNESS;
    if (alloc_seek_start > index) alloc_seek_start = index;
}
