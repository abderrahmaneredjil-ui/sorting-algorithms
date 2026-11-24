#include <stdlib.h>
#include "memtrack.h"


#define HEADER 16   

static size_t g_current = 0;
static size_t g_peak    = 0;

void *tracked_malloc(size_t size)
{
    char *raw = malloc(size + HEADER);
    if (raw == NULL) return NULL;
    *(size_t *)raw = size;
    g_current += size;
    if (g_current > g_peak) g_peak = g_current;
    return raw + HEADER;
}

void tracked_free(void *ptr)
{
    if (ptr == NULL) return;
    char *raw = (char *)ptr - HEADER;
    g_current -= *(size_t *)raw;
    free(raw);
}

void   mem_reset(void) { g_current = 0; g_peak = 0; }
size_t mem_peak(void)  { return g_peak; }
