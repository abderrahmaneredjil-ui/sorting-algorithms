# Sorting Algorithms: Implementation, Measurement and Comparison

- **Author:** *Redjil Abderrahmane* - **Group:** *DD2* - **Academic year:** 2026/2027
- **Module:** Algorithmique et structures de données (Dr Sabri Ghazi), Université Badji Mokhtar, Annaba

## 1. Language and measurement libraries
- **C (C11)**, compiled with `gcc` (version: gcc 16.2.0).
- **Time:** `QueryPerformanceCounter` (Windows) / `clock_gettime(CLOCK_MONOTONIC)` (Linux) for wall-clock time, and `clock()` for processor time. Chosen because they have sub-millisecond resolution; `clock()` alone is too coarse on Windows.
- **Memory:** two complementary measures.
  1. *Extra memory of the sort itself*: `memtrack.c` wraps `malloc`/`free` and records the **peak number of bytes alive** during the sort. It is exact and isolates one algorithm (the OS counters cannot, because they only ever go up during a process).
  2. *Whole-process peak* from the OS: `GetProcessMemoryInfo` (`PeakWorkingSetSize`) on Windows, `getrusage` (`ru_maxrss`) on Linux. Printed at the end of the big-array example.
- Limit: stack memory (recursion) is not counted by the first measure. Merge Sort and Quick Sort recursion is only about log2(n) levels deep (see section 4), so it is tiny.

## 2. Install, build and run
```
git clone https://github.com/abderrahmaneredjil-ui/sorting-algorithms.git
cd sorting-algorithms
make            # builds ./sorting   (Windows/MSYS2 UCRT64: same command, creates sorting.exe)
./sorting       # run from the project root so results/results.csv can be written
make test       # correctness tests (5 algorithms x 4 input types x 7 sizes vs. a reference)
python plot_results.py   # optional: charts in results/ (needs matplotlib)
```
Without `make`: `gcc -O2 -Wall -Wextra src/*.c -o sorting -lpsapi` on Windows (drop `-lpsapi` on Linux).

Menu: 1-5 run one algorithm (you choose size and input type), 6 compares all five on sizes 1 000 / 5 000 / 10 000 / 50 000 / 100 000 and writes `results/results.csv`, 7 is the big-array example (100 000 random integers, fixed seed 42), 8 lets you type your own array and run all five algorithms on it.

## 3. Project structure
```
src/main.c        menu and entry point
src/sorting.c/h   the five algorithms (no printing, no measuring)
src/benchmark.c/h time and memory measurement
src/memtrack.c/h  malloc/free wrapper that tracks peak memory
src/utils.c/h     array generation (own seeded generator), checks, safe input
tests/            correctness tests        results/   CSV and charts
```
Every algorithm sorts **its own copy** of the same data; only the sort call is timed; the average of 5 runs is reported (1 run above 10 000 elements, because a single run is already long). The random generator is a custom xorshift32 rather than `rand()`, because `RAND_MAX` is 32 767 on Windows and 2 147 483 647 on Linux: the same seed would give different arrays.

## 4. The algorithms
| Algorithm | Best | Average | Worst | Extra space | Notes |
|---|---|---|---|---|---|
| Bubble Sort | O(n) (early exit) | O(n^2) | O(n^2) | O(1) | swaps neighbours; stops if a pass has no swap |
| Selection Sort | O(n^2) | O(n^2) | O(n^2) | O(1) | always scans the whole unsorted part; at most n-1 swaps |
| Insertion Sort | O(n) | O(n^2) | O(n^2) | O(1) | very fast on nearly sorted data |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) | O(n) | one temporary buffer; stable |
| Quick Sort | O(n log n) | O(n log n) | O(n^2) | O(log n) stack | **median-of-three** pivot + Hoare partition; recursion only on the smaller part |

## 5. Results
Machine: Intel Core i5-6300U @ 2.40 GHz, 7.7 GB RAM, Windows 10 Pro (64-bit). Compiler: gcc 16.2.0 (MSYS2 UCRT64), compiled with -O2.
The table below shows my own results (random data, wall-clock time in ms); the full data is in `results/results.csv`.


| Algorithm | 1 000 | 5 000 | 10 000 | 50 000 | 100 000 |
|---|---|---|---|---|---|
| Bubble | 1.255 | 32.33 | 155.8 | 5 146 | skipped |
| Selection | 1.544 | 29.27 | 126.5 | 3 035 | skipped |
| Insertion | 0.224 | 3.166 | 14.16 | 337.4 | skipped |
| Merge | 0.067 | 0.419 | 0.909 | 5.439 | 11.59 |
| Quick | 0.062 | 0.378 | 0.843 | 4.821 | 11.87 |

Extra memory: Merge Sort uses 390.6 KB at n = 100 000 (exactly n x 4 bytes); the other four use none. Big array example (100 000 random integers, seed 42): Merge Sort 12.9 ms, Quick Sort 10.4 ms. For the three quadratic algorithms I used 20 000 elements instead: Bubble Sort 709 ms, Selection Sort 495 ms, Insertion Sort 56.5 ms. The peak memory of the whole process reported by Windows was 4 104 KB. The quadratic algorithms are skipped above 50 000 elements in the comparison table (constant `QUADRATIC_MAX` in `main.c`) because one Bubble Sort run already takes several seconds.

Other input types at n = 50 000 (wall-clock ms): sorted - Bubble 0.051, Insertion 0.138, Selection 3 141, Quick 0.529, Merge 1.339; reverse - Bubble 2 018, Selection 3 200, Insertion 704, Quick 1.006, Merge 1.968.

## 6. Critical analysis

**Theory vs. experiment**

The results match the theory well. When I made the array 5 times bigger (from 10 000 to 50 000 elements), Selection Sort and Insertion Sort became about 24 times slower (126 ms to 3 035 ms, and 14 ms to 337 ms). This is what O(n^2) predicts, because 5 x 5 = 25. Bubble Sort was a bit worse, about 33 times slower (156 ms to 5 146 ms). Merge Sort and Quick Sort became only about 6 times slower (0.9 ms to 5.4 ms, and 0.8 ms to 4.8 ms), which matches what O(n log n) predicts (about 6 times). At 100 000 elements, Merge Sort and Quick Sort still took only about 12 ms, while I skipped the three slow algorithms because they would take too long.

**Best algorithm by situation**

- *Random data:* Quick Sort was the fastest at almost every size and uses no extra memory. Merge Sort was a little slower (about 10 %) and uses extra memory (390.6 KB for 100 000 numbers), Merge Sort was a little slower (about 10 %) and uses extra memory (390.6 KB for 100 000 numbers), but its worst case is still O(n log n), and it is stable. and it is stable. At 100 000 elements the two were very close (11.59 ms and 11.87 ms in the table).
- *Already sorted data:* Bubble Sort (with the flag) and Insertion Sort were the best, with 0.051 ms and 0.138 ms for 50 000 elements. They only go through the array once. Selection Sort stayed very slow (3 141 ms).
- *Reverse data:* Quick Sort (1.0 ms) and Merge Sort (2.0 ms) were far better than the three quadratic algorithms (from 704 ms to 3 200 ms).
- In general, for big arrays I would choose Quick Sort, or Merge Sort if I need a guaranteed time or a stable sort. For small or almost sorted arrays, Insertion Sort is enough.

**Surprises**

- Selection Sort took about the same time on random, sorted and reverse data (3 035, 3 141 and 3 200 ms for 50 000 elements). The reason is that it always scans the whole unsorted part to find the smallest value, whatever the order.
- Bubble Sort was faster on reverse data (2 018 ms) than on random data (5 146 ms), even though reverse data is its worst case in theory. I did not test why. My guess is that on reverse data every comparison gives the same answer, so the processor can predict it, while on random data it often guesses wrong.
- Insertion Sort was about 2 times slower on reverse data (704 ms) than on random data (337 ms). This is expected, because on reverse data every element must move all the way to the left, and on random data it moves about halfway.
- From 5 000 elements, Bubble Sort was slower than Selection Sort on random data (for example 5 146 ms and 3 035 ms at 50 000 elements), because it makes many more swaps. At 1 000 elements it was the opposite (1.255 ms for Bubble and 1.544 ms for Selection), probably because the arrays are so small that the difference is hard to measure.

**Problems I faced**

- The three quadratic algorithms are too slow for big arrays. One Bubble Sort run on 50 000 numbers takes about 5 seconds, so I skipped them at 100 000 elements, and for the big array example I used 20 000 elements for them.
- The CPU time (`clock()`) shows 0 for very small arrays, because on Windows it only changes in steps of about 15 ms. For small arrays I trust the wall-clock time instead.
- For sizes above 10 000, each algorithm was measured only once because the runs are long. So small differences can be just noise, like Merge Sort and Quick Sort at 100 000 elements.
- When I recompiled, Windows said "Permission denied" because the old `sorting.exe` was still running. I had to close it first.
- The results are from one laptop (Intel Core i5-6300U), and the times change a little between runs, but the general picture stays the same.