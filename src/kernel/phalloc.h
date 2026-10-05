#ifndef PHALLOC_H
#define PHALLOC_H

#include <types.h>

void phalloc_init();

void *phalloc_addr(void *addr);

void *phalloc();

void phfree(void *addr);

#endif
