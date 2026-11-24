#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "benchmark.h"
#include "sorting.h"
#include "memtrack.h"
#include "utils.h"

#ifdef _WIN32
  #include <windows.h>
  #include <psapi.h>
#else
  #include <sys/resource.h>
#endif

const Algo ALGOS[NUM_ALGOS] = {
    { "Bubble Sort",    bubble_sort,    1 },
    { "Selection Sort", selection_sort, 1 },
    { "Insertion Sort", insertion_sort, 1 },
    { "Merge Sort",     merge_sort,     0 },
    { "Quick Sort",     quick_sort,     0 },
};


static double wall_now_ms(void)
{
#ifdef _WIN32
    LARGE_INTEGER freq, t;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart * 1000.0 / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
#endif
}

static double cpu_now_ms(void) { return (double)clock() * 1000.0 / CLOCKS_PER_SEC; }

BenchResult benchmark_run(SortFn fn, const int *original, int n, int runs)
{
    BenchResult r = { 0, 0, 0, 1 };
    int *work = malloc((size_t)n * sizeof(int));     
    if (work == NULL) { r.correct = 0; return r; }

    for (int k = 0; k < runs; k++) {
        memcpy(work, original, (size_t)n * sizeof(int));  
        mem_reset();
        double w0 = wall_now_ms(), c0 = cpu_now_ms();
        fn(work, n);                                        
        double w1 = wall_now_ms(), c1 = cpu_now_ms();
        r.wall_ms += w1 - w0;
        r.cpu_ms  += c1 - c0;
        double kb = (double)mem_peak() / 1024.0;
        if (kb > r.mem_kb) r.mem_kb = kb;
        if (!is_sorted(work, n)) r.correct = 0;
    }
    r.wall_ms /= runs;
    r.cpu_ms  /= runs;
    free(work);
    return r;
}

long process_peak_kb(void)
{
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof pmc))
        return (long)(pmc.PeakWorkingSetSize / 1024);
    return -1;
#else
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_maxrss;          
#endif
}
