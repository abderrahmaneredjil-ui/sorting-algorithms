CC     = gcc
CFLAGS = -O2 -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
SRC    = src/main.c src/sorting.c src/benchmark.c src/utils.c src/memtrack.c
LIB    =
ifeq ($(OS),Windows_NT)
  LIB = -lpsapi
  EXE = .exe
endif

all: sorting$(EXE)

sorting$(EXE): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LIB)

test: tests/test_sorting.c src/sorting.c src/benchmark.c src/utils.c src/memtrack.c
	$(CC) $(CFLAGS) tests/test_sorting.c src/sorting.c src/benchmark.c src/utils.c src/memtrack.c -o test_sorting$(EXE) $(LIB)
	./test_sorting$(EXE)

clean:
	rm -f sorting sorting.exe test_sorting test_sorting.exe
