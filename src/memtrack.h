#ifndef MEMTRACK_H
#define MEMTRACK_H
#include <stddef.h>


void  *tracked_malloc(size_t size);
void   tracked_free(void *ptr);
void   mem_reset(void);      
size_t mem_peak(void);       

#endif
