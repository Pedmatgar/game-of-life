#include <stdlib.h>
#include <time.h>
#include "animations.h"
#include "life.h"

/*
 * Function: delays
 * ----------------------------
 * Delays the current thread.
 * 
 * delay_in_seconds: delay time
 */
void delays(int delay_in_seconds)
{
	// Use nanosleep for real wall-clock delay (clock() measures CPU time, not elapsed time)
	struct timespec ts = { .tv_sec = delay_in_seconds, .tv_nsec = 0 };
	struct timespec remaining;
	// Retry if interrupted by a signal
	while (nanosleep(&ts, &remaining) == -1)
		ts = remaining;
}

void animate_world(int n, int world[][VIRTUAL_MAX_COLS + 2],
				   int rows_count, int cols_count,
				   int aux_world[][MAX_COLS],
				   const int rule[RULE_SIZE], int delay_in_seconds)
{
	system("clear"); // Clear Screen in Unix

	print_world(world, rows_count, cols_count);

	for (int i = 0; i < n; i++)
	{
		delays(delay_in_seconds);
		system("clear");
		update_world(world, rows_count, cols_count, aux_world, rule);
		print_world(world, rows_count, cols_count);
	}
}
