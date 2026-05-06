#include <stdio.h>
#include "life.h"

#define PERF_ROWS VIRTUAL_MAX_COLS
#define PERF_COLS VIRTUAL_MAX_COLS
#define PERF_ITERS 1000

/*
 * Game of life performance tests.
 * Each scenario runs PERF_ITERS generations on a PERF_ROWS x PERF_COLS world
 * and reports total time, average time per generation, and throughput.
 */

static void report(const char *name, int iters, int rows, int cols, double total)
{
	double avg = total / iters;
	double cells_per_sec = (double)iters * rows * cols / total;
	printf("%-30s  iters=%d  total=%.6f s  avg=%.9f s  throughput=%.0f cells/s\n",
		   name, iters, total, avg, cells_per_sec);
}

static void fill_dense(int world[][MAX_COLS], int rows, int cols)
{
	for (int r = 1; r <= rows; r++)
		for (int c = 1; c <= cols; c++)
			set_cell(world, r, c, 1);
}

static void fill_checkerboard(int world[][MAX_COLS], int rows, int cols)
{
	for (int r = 1; r <= rows; r++)
		for (int c = 1; c <= cols; c++)
			set_cell(world, r, c, (r + c) % 2);
}

static void fill_sparse(int world[][MAX_COLS], int rows, int cols)
{
	clear_world(world, rows, cols);
	// Single glider-like cross in the center
	int mr = rows / 2 + 1;
	int mc = cols / 2 + 1;
	for (int r = 1; r <= rows; r++)
		set_cell(world, r, mc, 1);
	for (int c = 1; c <= cols; c++)
		set_cell(world, mr, c, 1);
}

int main(void)
{
	int rule[RULE_SIZE] = {3, 2, 3}; // Default Conway rule

	// Effective array size: (PERF_ROWS + 2) x MAX_COLS
	int world[(PERF_ROWS + 2)][MAX_COLS];
	int aux[(PERF_ROWS + 2)][MAX_COLS];

	double total;

	printf("Performance benchmark: %d x %d world, %d iterations each\n\n",
		   PERF_ROWS, PERF_COLS, PERF_ITERS);

	// -------------------------------------------------------------------------------
	// Scenario 1: Dense world (all cells alive)

	fill_dense(world, PERF_ROWS, PERF_COLS);
	total = update_world_n_generations_timed(PERF_ITERS, world, PERF_ROWS, PERF_COLS, aux, rule, NULL);
	report("dense", PERF_ITERS, PERF_ROWS, PERF_COLS, total);

	// -------------------------------------------------------------------------------
	// Scenario 2: Checkerboard pattern

	fill_checkerboard(world, PERF_ROWS, PERF_COLS);
	total = update_world_n_generations_timed(PERF_ITERS, world, PERF_ROWS, PERF_COLS, aux, rule, NULL);
	report("checkerboard", PERF_ITERS, PERF_ROWS, PERF_COLS, total);

	// -------------------------------------------------------------------------------
	// Scenario 3: Sparse world (cross in center)

	fill_sparse(world, PERF_ROWS, PERF_COLS);
	total = update_world_n_generations_timed(PERF_ITERS, world, PERF_ROWS, PERF_COLS, aux, rule, NULL);
	report("sparse (cross)", PERF_ITERS, PERF_ROWS, PERF_COLS, total);

	return 0;
}
