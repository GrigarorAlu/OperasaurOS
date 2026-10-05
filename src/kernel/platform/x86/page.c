#include "../page.h"

#include "../../phalloc.h"
#include <mem.h>

#define PAGE_TABLE_SIZE 1024

static u32 page_dir[PAGE_TABLE_SIZE] __attribute__((aligned(4096))) = {};
static u16 page_table_counter[PAGE_TABLE_SIZE] = {};

void page_init()
{
    mem_set(page_dir, PAGE_SIZE, 0);
    u32 *page_table = phalloc();
    page_dir[0] = (usz)page_table | PAGE_WRITE | PAGE_PRESENT;
    for (u16 i = 0; i < PAGE_TABLE_SIZE; i++) page_table[i] = i * PAGE_SIZE | PAGE_WRITE | PAGE_PRESENT;

    __asm__ volatile("movl $page_dir, %eax; movl %eax, %cr3");
    __asm__ volatile("movl %cr0, %eax; orl $0x80000000, %eax; movl %eax, %cr0");
}

void *page_map(void *physical, void *virtual, u8 flags)
{
    u32 dir_index = (usz)virtual >> 22;
    u32 *dir_entry = &page_dir[dir_index];

    void *addr = phalloc_addr(physical);
    if (addr == null) return null;

    u32 *table_entry;
    if (*dir_entry & PAGE_PRESENT)
    {
        u32 *page_table = (u32 *)(*dir_entry & 0xFFFFF000);
        table_entry = (u32 *)page_table[(usz)virtual >> 12 & 0x3FF];
        if (*table_entry & PAGE_PRESENT)
        {
            phfree(addr);
            return null;
        }
    }
    else
    {
        u32 *page_table = phalloc();
        table_entry = (u32 *)page_table[(usz)virtual >> 12 & 0x3FF];
        *dir_entry = (usz)table_entry | flags | PAGE_PRESENT;
    }

    *table_entry = (usz)addr | flags | PAGE_PRESENT;
    page_table_counter[dir_index]++;

    return addr;
}

void page_unmap(void *virtual)
{
    usz dir_index = (usz)virtual >> 22;
    u32 *dir_entry = &page_dir[dir_index];
    if (!(*dir_entry & PAGE_PRESENT)) return;

    u32 *page_table = (u32 *)(*dir_entry & 0xFFFFF000);
    u32 *table_entry = &page_table[(usz)virtual >> 12 & 0x3FF];
    if (!(*table_entry & PAGE_PRESENT)) return;

    phfree((void *)(*table_entry & 0xFFFFF000));

    *table_entry = 0;
    if (--page_table_counter[dir_index] == 0)
    {
        phfree(page_table);
        *dir_entry = 0;
    }
}
