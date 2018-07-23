#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "grid.h"
#include "vector_3.h"

void simulation_init(char *fp, union vector_3i *divs, union vector_3d *grid_start, union vector_3d *grid_end, double time_limit);
void simulation_run();
void simulation_cleanup();

// Returns a copy of the final sphere state to be used for comparison
struct sphere_s *run(char *fp, union vector_3i *divs, union vector_3d *grid_start, union vector_3d *grid_end) {
	printf("Running with %d x slices, %d y slces and %d z slices\n", divs->x, divs->y, divs->z);
	simulation_init(fp, divs, grid_start, grid_end, 10.0);
#ifdef _WIN32
	clock_t start = clock();
#endif
	simulation_run();
#ifdef _WIN32
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Time taken in seconds: %f\n", seconds);
#endif
#ifdef RECORD_STATS
	printf("Number of sphere on sphere collisions: %d\n", grid->num_two_sphere_collisions);
	printf("Number of collisions with grid boundary: %d\n", grid->num_grid_collisions);
	printf("Number of transfers between sectors (if used): %d\n", grid->num_sector_transfers);
#endif
	struct sphere_s *copy = malloc(NUM_SPHERES * sizeof(struct sphere_s));
	memcpy(copy, spheres, NUM_SPHERES * sizeof(struct sphere_s));
	simulation_cleanup();
	return copy;
}

void compare_results(struct sphere_s *results_1, struct sphere_s *results_2) {
	double max_pos_err = 0.0;
	double max_vel_err = 0.0;
	int i;
	for (i = 0; i < NUM_SPHERES; i++) {
		struct sphere_s s1 = results_1[i];
		struct sphere_s s2 = results_2[i];
		enum axis a;
		for (a = X_AXIS; a <= Z_AXIS; a++) {
			if (fabs(s1.pos.vals[a] - s2.pos.vals[a]) > max_pos_err) {
				max_pos_err = fabs(s1.pos.vals[a] - s2.pos.vals[a]);
			}
			if (fabs(s1.vel.vals[a] - s2.vel.vals[a]) > max_vel_err) {
				max_vel_err = fabs(s1.vel.vals[a] - s2.vel.vals[a]);
			}
		}
	}
	printf("vel abs err: %.17g\n", max_vel_err);
	printf("pos abs err: %.17g\n", max_pos_err);
}

int main(void) {
	union vector_3i divs = { .x = 3, .y = 3, .z = 1 };
	union vector_3d grid_start = { .x = 0.0,.y = 0.0,.z = 0.0 };
	union vector_3d grid_end = { .x = 1000.0,.y = 1000.0, .z = 1000.0 };
	struct sphere_s *results_2 = run("dd.bin", &divs, &grid_start, &grid_end);
	divs.x = 1; divs.y = 1;
	struct sphere_s *results_1 = run("normal.bin", &divs, &grid_start, &grid_end);
	compare_results(results_1, results_2);
	printf("Press enter to exit...\n");
	getchar();
	return 0;
}