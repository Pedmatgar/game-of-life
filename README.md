# Measuring Execution Time in Game of Life

This tutorial explains how to measure the execution time of the Game of Life simulation using the timing API provided by the `life` module.

---

## Prerequisites

- GCC (or any C99-compatible compiler)
- `make`
- A Unix-like system (Linux, macOS) — `CLOCK_MONOTONIC` from `<time.h>` is used internally

---

## Build

The `life` module and its tests are built with the Makefile inside the `life` directory:

```
    .../game-of-life/life$ make
```

To build and run the animation app:

```
    .../game-of-life/animations$ make
```

---

## Timing API Overview

Include the header and link `life.c` in your program:

```c
#include "life.h"
```

Two functions measure elapsed wall-clock time using `clock_gettime(CLOCK_MONOTONIC, ...)`:

| Function | What it measures |
|---|---|
| `update_world_timed(...)` | Time for a **single** generation update |
| `update_world_n_generations_timed(...)` | Total time for **n** generation updates, with optional per-step breakdown |

Both functions advance the world normally and return the elapsed time in **seconds** as a `double`.

---

## Tutorial: Timing a Single Generation

```c
#include <stdio.h>
#include "life.h"

int main(void)
{
    /* 1. Declare the world (virtual size rows x cols; array needs rows+2 = 12 rows) */
    int rows = 10, cols = 10;
    int world[12][MAX_COLS];
    int world_aux[12][MAX_COLS];

    /* 2. Initialise all cells to dead */
    clear_world(world, rows, cols);

    /* 3. Set up an initial pattern (a simple blinker) */
    set_cell(world, 5, 4, 1);
    set_cell(world, 5, 5, 1);
    set_cell(world, 5, 6, 1);

    /* 4. Choose the standard Conway rule {maxNeighbors, minNeighbors, neighborsToBorn} */
    int rule[RULE_SIZE] = {3, 2, 3};

    /* 5. Advance one generation and measure the time */
    double elapsed = update_world_timed(world, rows, cols, world_aux, rule);

    printf("One generation took %.9f seconds\n", elapsed);

    return 0;
}
```

Compile and run:

```
gcc -Wall -Ilife/include life/src/life.c my_timing.c -o my_timing
./my_timing
```

Expected output (times will vary by hardware):

```
One generation took 0.000002341 seconds
```

---

## Tutorial: Timing Multiple Generations

### Total time only

Pass `NULL` as the last argument when you only need the grand total:

```c
int n = 100;
double total = update_world_n_generations_timed(
    n, world, rows, cols, world_aux, rule, NULL);

printf("100 generations took %.9f seconds (%.6f ms)\n",
       total, total * 1000.0);
```

### Per-step breakdown

Allocate an array of `n` doubles to capture each individual generation time:

```c
int n = 5;
double step_times[5];

double total = update_world_n_generations_timed(
    n, world, rows, cols, world_aux, rule, step_times);

for (int i = 0; i < n; i++)
    printf("  Step %d: %.9f s\n", i + 1, step_times[i]);

printf("Total: %.9f s\n", total);
```

The sum of all `step_times[i]` equals the returned `total`.

---

## Understanding the World Layout

The world array is padded by one extra row and column on every side to simplify the
wrap-around boundary logic.  A virtual grid of `R` rows × `C` cols therefore requires
an array of size `(R+2) × (MAX_COLS)`:

```
  world[0][*]          <- padding row (top)
  world[1..R][1..C]    <- live cells
  world[R+1][*]        <- padding row (bottom)
  world[*][0]          <- padding column (left)
  world[*][C+1]        <- padding column (right)
```

`MAX_COLS` is defined in `life.h` as `VIRTUAL_MAX_COLS + 2` (currently 73).

---

## Rule Format

The `rule` array has three elements:

```c
int rule[RULE_SIZE] = {maxNeighbors, minNeighbors, neighborsToBorn};
```

| Index | Meaning | Conway value |
|---|---|---|
| 0 | `maxNeighbors` — maximum live neighbors for a live cell to survive | 3 |
| 1 | `minNeighbors` — minimum live neighbors for a live cell to survive | 2 |
| 2 | `neighborsToBorn` — exact live neighbors needed to birth a dead cell | 3 |

---

## Running the Existing Tests

The `life_test.c` file already exercises both timing functions and prints per-step and
total times.  Build and run it with:

```
    .../game-of-life/life$ make
```

Look for the sections labelled **Testing update_world_timed** and
**Testing update_world_n_generations_timed** in the output.

---

## Specifying the Number of Threads

The simulation uses [OpenMP](https://www.openmp.org/) to parallelize the per-generation
cell updates across multiple CPU cores.  By default the benchmark runs with **1 thread**
(sequential execution).

### Running with multiple threads

Pass `--threads N` (or the short form `-t N`) when invoking the built binary directly:

```
.../game-of-life/life/build/life_test --threads 4
.../game-of-life/life/build/life_test -t 2
```

The header line of the benchmark output confirms the active thread count:

```
Performance benchmark: 71 x 71 world, 1000 iterations each, 4 thread(s)
```

### Building and running in one step via make

`make` compiles and immediately runs the binary.  The run arguments can be forwarded
through `ARGS`:

```
.../game-of-life/life$ make ARGS="--threads 4"
```

> **Tip:** set `N` to the number of physical (or logical) CPU cores on your machine for
> the best throughput.  A value larger than the available cores will not improve
> performance and may slightly degrade it.

### Using the API from your own code

```c
#include "life.h"

set_num_threads(4);   /* must be called before any update_world* call */
```

`set_num_threads(n)` delegates to `omp_set_num_threads(n)` and is a no-op for values
less than 1.
