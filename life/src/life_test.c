#include <stdio.h>
#include "life.h"
#ifdef USE_MPI
#include <mpi.h>
#endif

// Square world: PERF_SIZE x PERF_SIZE (uses VIRTUAL_MAX_COLS as the single
// dimension limit since the grid is intentionally square)
#define PERF_SIZE  VIRTUAL_MAX_COLS
#define PERF_ITERS 1000

/*
 * Game of life performance tests.
 * Each scenario runs PERF_ITERS generations on a PERF_SIZE x PERF_SIZE world
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
	// Cross (plus-sign) centered in the world
	int mr = rows / 2 + 1;
	int mc = cols / 2 + 1;
	for (int r = 1; r <= rows; r++)
		set_cell(world, r, mc, 1);
	for (int c = 1; c <= cols; c++)
		set_cell(world, mr, c, 1);
}

int main(void)
{
#ifdef USE_MPI
	MPI_Init(NULL, NULL);
	int mpi_rank = 0, mpi_size = 1;
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
#endif

	int rule[RULE_SIZE] = {3, 2, 3}; // Default Conway rule

	// Effective array size: (PERF_SIZE + 2) x MAX_COLS
	int world[(PERF_SIZE + 2)][MAX_COLS];
	int aux[(PERF_SIZE + 2)][MAX_COLS];

	double total;

#ifdef USE_MPI
	if (mpi_rank == 0)
		printf("Performance benchmark (MPI): %d x %d world, %d iterations each, ranks=%d\n\n",
			   PERF_SIZE, PERF_SIZE, PERF_ITERS, mpi_size);
#else
	printf("Performance benchmark: %d x %d world, %d iterations each\n\n",
		   PERF_SIZE, PERF_SIZE, PERF_ITERS);
#endif

	// -------------------------------------------------------------------------------
	// Scenario 1: Dense world (all cells alive)

	fill_dense(world, PERF_SIZE, PERF_SIZE);
#ifdef USE_MPI
	total = update_world_n_generations_mpi_timed(PERF_ITERS, world, PERF_SIZE, PERF_SIZE, aux, rule);
#else
	total = update_world_n_generations_timed(PERF_ITERS, world, PERF_SIZE, PERF_SIZE, aux, rule, NULL);
#endif
#ifdef USE_MPI
	if (mpi_rank == 0)
#endif
	report("dense", PERF_ITERS, PERF_SIZE, PERF_SIZE, total);

	// -------------------------------------------------------------------------------
	// Scenario 2: Checkerboard pattern

	fill_checkerboard(world, PERF_SIZE, PERF_SIZE);
#ifdef USE_MPI
	total = update_world_n_generations_mpi_timed(PERF_ITERS, world, PERF_SIZE, PERF_SIZE, aux, rule);
#else
	total = update_world_n_generations_timed(PERF_ITERS, world, PERF_SIZE, PERF_SIZE, aux, rule, NULL);
#endif
#ifdef USE_MPI
	if (mpi_rank == 0)
#endif
	report("checkerboard", PERF_ITERS, PERF_SIZE, PERF_SIZE, total);

	// -------------------------------------------------------------------------------
	// Scenario 3: Sparse world (cross in center)

	fill_sparse(world, PERF_SIZE, PERF_SIZE);
#ifdef USE_MPI
	total = update_world_n_generations_mpi_timed(PERF_ITERS, world, PERF_SIZE, PERF_SIZE, aux, rule);
#else
	total = update_world_n_generations_timed(PERF_ITERS, world, PERF_SIZE, PERF_SIZE, aux, rule, NULL);
#endif
#ifdef USE_MPI
	if (mpi_rank == 0)
#endif
	report("sparse (cross)", PERF_ITERS, PERF_SIZE, PERF_SIZE, total);

#ifdef USE_MPI
	MPI_Finalize();
#endif
	return 0;
}
