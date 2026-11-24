/* Menu and program entry point. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "benchmark.h"
#include "utils.h"

#define QUADRATIC_MAX 50000   
#define BIG_N         100000  
#define BIG_QUAD_N    20000   
#define MAX_N         1000000

static int runs_for(int n) { return n <= 10000 ? 5 : 1; }   

static void print_header(void)
{
    printf("%-16s %10s %12s %12s %14s %5s\n",
           "Algorithm", "Array size", "Wall (ms)", "CPU (ms)", "Memory (KB)", "Rank");
}

static int *make_array(int n, InputType t)
{
    int *a = malloc((size_t)n * sizeof(int));
    if (a == NULL) { printf("Not enough memory for %d integers.\n", n); return NULL; }
    fill_array(a, n, t, SEED);
    return a;
}


static void run_single(int idx)
{
    const Algo *algo = &ALGOS[idx];
    int n = read_int("Array size (1 - 1000000): ", 1, MAX_N);
    int t = read_int("Input type (1 random, 2 sorted, 3 reverse, 4 duplicates): ", 1, 4);
    if (algo->quadratic && n > QUADRATIC_MAX)
        printf("  Note: %s is O(n^2), this may take a long time.\n", algo->name);

    int *a = make_array(n, (InputType)t);
    if (a == NULL) return;

    printf("\n%s on %d %s integers (seed %u)\n", algo->name, n, input_name((InputType)t), SEED);
    printf("Before: "); print_preview(a, n);

    int runs = runs_for(n);
    BenchResult r = benchmark_run(algo->fn, a, n, runs);

    if (n <= 20000) {                         /* untimed extra sort, only to show the result */
        algo->fn(a, n);
        printf("After:  "); print_preview(a, n);
    }
    printf("Wall-clock time : %.3f ms (average of %d runs)\n", r.wall_ms, runs);
    printf("CPU time        : %.3f ms\n", r.cpu_ms);
    printf("Peak extra mem  : %.1f KB\n", r.mem_kb);
    printf("Correctly sorted: %s\n\n", r.correct ? "yes" : "NO - BUG!");
    free(a);
}


static void compare_one_type(InputType t, FILE *csv)
{
    static const int sizes[] = { 1000, 5000, 10000, 50000, 100000 };
    printf("\n=== Input type: %s ===\n", input_name(t));
    print_header();

    for (size_t s = 0; s < sizeof sizes / sizeof sizes[0]; s++) {
        int n = sizes[s];
        int *a = make_array(n, t);
        if (a == NULL) return;

        BenchResult res[NUM_ALGOS];
        int skipped[NUM_ALGOS];
        for (int i = 0; i < NUM_ALGOS; i++) {
            skipped[i] = ALGOS[i].quadratic && n > QUADRATIC_MAX;
            if (!skipped[i]) res[i] = benchmark_run(ALGOS[i].fn, a, n, runs_for(n));
        }
        for (int i = 0; i < NUM_ALGOS; i++) {
            if (skipped[i]) {
                printf("%-16s %10d %12s %12s %14s %5s\n", ALGOS[i].name, n, "skipped", "-", "-", "-");
                continue;
            }
            int rank = 1;                                  /* rank 1 = fastest */
            for (int j = 0; j < NUM_ALGOS; j++)
                if (!skipped[j] && res[j].wall_ms < res[i].wall_ms) rank++;
            printf("%-16s %10d %12.3f %12.3f %14.1f %5d%s\n", ALGOS[i].name, n,
                   res[i].wall_ms, res[i].cpu_ms, res[i].mem_kb, rank,
                   res[i].correct ? "" : "  (NOT SORTED!)");
            if (csv)
                fprintf(csv, "%s,%s,%d,%.3f,%.3f,%.1f,%d\n", input_name(t), ALGOS[i].name, n,
                        res[i].wall_ms, res[i].cpu_ms, res[i].mem_kb, rank);
        }
        free(a);
    }
}

static void compare_all(void)
{
    int choice = read_int("Input type (1 random, 2 sorted, 3 reverse, 4 duplicates, 5 all four): ", 1, 5);
    FILE *csv = fopen("results/results.csv", "w");
    if (csv) fprintf(csv, "input,algorithm,size,wall_ms,cpu_ms,memory_kb,rank\n");
    else     printf("  (could not create results/results.csv - run the program from the project root)\n");

    printf("Quadratic algorithms are skipped above %d elements (see README).\n", QUADRATIC_MAX);
    if (choice == 5) for (int t = INPUT_RANDOM; t <= INPUT_DUPLICATES; t++) compare_one_type((InputType)t, csv);
    else             compare_one_type((InputType)choice, csv);

    if (csv) { fclose(csv); printf("\nResults saved to results/results.csv\n"); }
    printf("\n");
}


static void big_example(void)
{
    printf("\nBig array example: random integers, fixed seed %u\n", SEED);

    int *big = make_array(BIG_N, INPUT_RANDOM);
    if (big == NULL) return;
    printf("\n-- %d elements: Merge Sort and Quick Sort --\n", BIG_N);
    print_header();
    for (int i = 3; i < NUM_ALGOS; i++) {
        BenchResult r = benchmark_run(ALGOS[i].fn, big, BIG_N, 3);
        printf("%-16s %10d %12.3f %12.3f %14.1f %5s\n", ALGOS[i].name, BIG_N, r.wall_ms, r.cpu_ms, r.mem_kb, "");
    }
    free(big);

    int *mid = make_array(BIG_QUAD_N, INPUT_RANDOM);
    if (mid == NULL) return;
    printf("\n-- %d elements: the three quadratic algorithms (too slow for %d) --\n", BIG_QUAD_N, BIG_N);
    print_header();
    for (int i = 0; i < 3; i++) {
        BenchResult r = benchmark_run(ALGOS[i].fn, mid, BIG_QUAD_N, 1);
        printf("%-16s %10d %12.3f %12.3f %14.1f %5s\n", ALGOS[i].name, BIG_QUAD_N, r.wall_ms, r.cpu_ms, r.mem_kb, "");
    }
    free(mid);

    long peak = process_peak_kb();
    if (peak >= 0) printf("\nPeak memory of the whole process (from the OS): %ld KB\n", peak);
    printf("\n");
}


static void run_custom(void)
{
    char line[16384];
    printf("Type your integers separated by spaces (or commas), then press Enter:\n> ");
    if (fgets(line, sizeof line, stdin) == NULL) return;
    if (strchr(line, '\n') == NULL && strlen(line) == sizeof line - 1) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) { }      /* throw away the rest of the line */
        printf("  That line is too long (limit about 16000 characters).\n\n");
        return;
    }

    int cap = 64, n = 0;
    int *a = malloc((size_t)cap * sizeof(int));
    if (a == NULL) return;

    char *p = line;
    for (;;) {
        while (*p == ' ' || *p == '\t' || *p == ',' || *p == '\r' || *p == '\n') p++;   /* skip separators */
        if (*p == '\0') break;                                                          /* end of the line */
        char *end;
        errno = 0;
        long v = strtol(p, &end, 10);
        if (end == p || errno == ERANGE || v > INT_MAX || v < INT_MIN) {
            printf("  Invalid value near \"%.10s\": only whole numbers are accepted.\n\n", p);
            free(a);
            return;
        }
        if (n == cap) {                                   
            cap *= 2;
            int *bigger = realloc(a, (size_t)cap * sizeof(int));
            if (bigger == NULL) { free(a); return; }
            a = bigger;
        }
        a[n++] = (int)v;
        p = end;
    }

    if (n == 0) { printf("  You did not type any number.\n\n"); free(a); return; }

    printf("\nYour array (%d numbers):\n  ", n);
    if (n <= 50) print_array(a, n); else print_preview(a, n);

    printf("\n%-16s %12s %12s %14s\n", "Algorithm", "Wall (ms)", "CPU (ms)", "Memory (KB)");
    int *sorted = malloc((size_t)n * sizeof(int));
    if (sorted == NULL) { free(a); return; }
    for (int i = 0; i < NUM_ALGOS; i++) {
        BenchResult r = benchmark_run(ALGOS[i].fn, a, n, runs_for(n));
        memcpy(sorted, a, (size_t)n * sizeof(int));
        ALGOS[i].fn(sorted, n);                           
        printf("%-16s %12.5f %12.5f %14.1f%s\n", ALGOS[i].name, r.wall_ms, r.cpu_ms, r.mem_kb,
               r.correct ? "" : "  (NOT SORTED!)");
        printf("  -> "); if (n <= 50) print_array(sorted, n); else print_preview(sorted, n);
    }
    printf("\n(For tiny arrays the times are almost 0: the computer is too fast to measure them.)\n\n");
    free(a);
    free(sorted);
}

int main(void)
{
    for (;;) {
        printf("=========== SORTING ALGORITHMS ===========\n"
               "1. Bubble Sort\n2. Selection Sort\n3. Insertion Sort\n4. Merge Sort\n5. Quick Sort\n"
               "6. Compare all algorithms\n7. Big array example (100 000 integers)\n8. Type my own array and run all 5 algorithms\n0. Exit\n");
        int choice = read_int("Your choice: ", 0, 8);
        if (choice == 0) { printf("Goodbye!\n"); return 0; }
        if (choice >= 1 && choice <= 5) run_single(choice - 1);
        else if (choice == 6)           compare_all();
        else if (choice == 7)           big_example();
        else                            run_custom();
    }
}
