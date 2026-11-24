
#include <stdio.h>
#include <stdlib.h>
#include "../src/sorting.h"
#include "../src/benchmark.h"
#include "../src/utils.h"

static int cmp_int(const void *x, const void *y)
{
    int a = *(const int *)x, b = *(const int *)y;
    return (a > b) - (a < b);
}

int main(void)
{
    const int sizes[] = { 0, 1, 2, 3, 10, 101, 1000 };
    int failures = 0, checks = 0;

    for (int a = 0; a < NUM_ALGOS; a++)
        for (int t = INPUT_RANDOM; t <= INPUT_DUPLICATES; t++)
            for (size_t s = 0; s < sizeof sizes / sizeof sizes[0]; s++) {
                int n = sizes[s];
                int *x = malloc((n + 1) * sizeof(int)), *ref = malloc((n + 1) * sizeof(int));
                fill_array(x, n, (InputType)t, 7u);
                for (int i = 0; i < n; i++) ref[i] = x[i];
                qsort(ref, n, sizeof(int), cmp_int);   
                ALGOS[a].fn(x, n);
                int ok = 1;
                for (int i = 0; i < n; i++) if (x[i] != ref[i]) ok = 0;
                checks++;
                if (!ok) { failures++; printf("FAIL: %s, %s, n=%d\n", ALGOS[a].name, input_name((InputType)t), n); }
                free(x); free(ref);
            }

    printf("%d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
