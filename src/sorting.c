
#include <stddef.h>
#include "sorting.h"
#include "memtrack.h"

static void swap(int *x, int *y) { int t = *x; *x = *y; *y = t; }

/* ---------- 1. Bubble sort (with early exit) ---------- */
void bubble_sort(int a[], int n)
{
    for (int pass = 0; pass < n - 1; pass++) {
        int swapped = 0;                      
        for (int i = 0; i < n - 1 - pass; i++) {   
            if (a[i] > a[i + 1]) {
                swap(&a[i], &a[i + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;                   
    }
}

/* ---------- 2. Selection sort ---------- */
void selection_sort(int a[], int n)
{
    for (int i = 0; i < n - 1; i++) {
        int min = i;                          
        for (int j = i + 1; j < n; j++)
            if (a[j] < a[min]) min = j;
        if (min != i) swap(&a[i], &a[min]);
    }
}

/* ---------- 3. Insertion sort ---------- */
void insertion_sort(int a[], int n)
{
    for (int i = 1; i < n; i++) {
        int key = a[i];                        
        int j = i - 1;
        while (j >= 0 && a[j] > key) {         
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

/* ---------- 4. Merge sort ---------- */

static void merge(int a[], int tmp[], int lo, int mid, int hi)
{
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi)
        tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];   
    while (i <= mid) tmp[k++] = a[i++];
    while (j <= hi)  tmp[k++] = a[j++];
    for (k = lo; k <= hi; k++) a[k] = tmp[k];
}

static void merge_rec(int a[], int tmp[], int lo, int hi)
{
    if (lo >= hi) return;                      
    int mid = lo + (hi - lo) / 2;
    merge_rec(a, tmp, lo, mid);
    merge_rec(a, tmp, mid + 1, hi);
    merge(a, tmp, lo, mid, hi);
}

void merge_sort(int a[], int n)
{
    if (n < 2) return;
    int *tmp = tracked_malloc((size_t)n * sizeof(int));   
    if (tmp == NULL) return;
    merge_rec(a, tmp, 0, n - 1);
    tracked_free(tmp);
}

/* ---------- 5. Quick sort ----------*/

static int median_of_three(int a[], int lo, int hi)
{
    int mid = lo + (hi - lo) / 2;
    int x = a[lo], y = a[mid], z = a[hi];
    if ((x <= y && y <= z) || (z <= y && y <= x)) return y;
    if ((y <= x && x <= z) || (z <= x && x <= y)) return x;
    return z;
}

static void quick_rec(int a[], int lo, int hi)
{
    while (lo < hi) {
        int pivot = median_of_three(a, lo, hi);
        int i = lo, j = hi;
        while (i <= j) {
            while (a[i] < pivot) i++;
            while (a[j] > pivot) j--;
            if (i <= j) { swap(&a[i], &a[j]); i++; j--; }
        }
        
        if (j - lo < hi - i) { quick_rec(a, lo, j); lo = i; }
        else                 { quick_rec(a, i, hi); hi = j; }
    }
}

void quick_sort(int a[], int n)
{
    if (n > 1) quick_rec(a, 0, n - 1);
}
