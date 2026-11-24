#ifndef BENCHMARK_H
#define BENCHMARK_H

typedef void (*SortFn)(int a[], int n);

typedef struct {
    const char *name;
    SortFn      fn;
    int         quadratic;   
} Algo;

#define NUM_ALGOS 5
extern const Algo ALGOS[NUM_ALGOS];

typedef struct {
    double wall_ms;    
    double cpu_ms;      
    double mem_kb;      
    int    correct;     
} BenchResult;

BenchResult benchmark_run(SortFn fn, const int *original, int n, int runs);

long process_peak_kb(void);

#endif
