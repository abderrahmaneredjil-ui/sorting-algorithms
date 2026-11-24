#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"


static unsigned next_random(unsigned *state)
{
    unsigned x = *state;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    return *state = x;
}

const char *input_name(InputType t)
{
    switch (t) {
        case INPUT_RANDOM:     return "random";
        case INPUT_SORTED:     return "sorted";
        case INPUT_REVERSE:    return "reverse";
        case INPUT_DUPLICATES: return "duplicates";
    }
    return "?";
}

void fill_array(int a[], int n, InputType t, unsigned seed)
{
    unsigned state = seed ? seed : 1u;          
    for (int i = 0; i < n; i++) {
        switch (t) {
            case INPUT_RANDOM:     a[i] = (int)(next_random(&state) % 1000000u); break;
            case INPUT_SORTED:     a[i] = i;                                      break;
            case INPUT_REVERSE:    a[i] = n - i;                                  break;
            case INPUT_DUPLICATES: a[i] = (int)(next_random(&state) % 100u);      break; 
        }
    }
}

int is_sorted(const int a[], int n)
{
    for (int i = 1; i < n; i++)
        if (a[i - 1] > a[i]) return 0;
    return 1;
}

void print_preview(const int a[], int n)
{
    int shown = n < 10 ? n : 10;
    printf("[");
    for (int i = 0; i < shown; i++) printf("%s%d", i ? ", " : "", a[i]);
    printf("%s]\n", n > shown ? ", ..." : "");
}

void print_array(const int a[], int n)
{
    printf("[");
    for (int i = 0; i < n; i++) printf("%s%d", i ? ", " : "", a[i]);
    printf("]\n");
}


int read_int(const char *prompt, int min, int max)
{
    char line[64];
    for (;;) {
        printf("%s", prompt);
        if (fgets(line, sizeof line, stdin) == NULL) return min;   
        char *end;
        long v = strtol(line, &end, 10);
        while (*end == ' ' || *end == '\t') end++;
        if (end != line && (*end == '\n' || *end == '\0') && v >= min && v <= max)
            return (int)v;
        printf("  Invalid input: please type a whole number between %d and %d.\n", min, max);
    }
}
