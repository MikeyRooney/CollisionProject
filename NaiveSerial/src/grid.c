#include <float.h>
#include <stdlib.h>

#include "collision.h"
#include "grid.h"

/*
TODO figure out why this breaks right away
probably because time to next event is 0 as they are already touching
NUM_SPHERES = 1000;
spheres = calloc(NUM_SPHERES, sizeof(struct sphere_s));
double x = 2.0;
double y = 2.0;
int i, j;
int count = 0;
for (i = 0; i < 10; i++) {
for (j = 0; j < 100; j++) {
spheres[count].pos.x = x;
spheres[count].pos.y = y;
spheres[count].pos.z = 10.0;
spheres[count].vel.x = rand() / (RAND_MAX + 1.0);
spheres[count].vel.y = rand() / (RAND_MAX + 1.0);
spheres[count].vel.z = rand() / (RAND_MAX + 1.0);
spheres[count].mass = 1.0;
spheres[count].radius = 1.0;
y = y + 2.0;
count++;
}
x = x + 2.0;
}
*/

// Generates spheres with random velocities;
// Position is hardcoded for now
static void init_spheres() {
	NUM_SPHERES = 1000;
	spheres = calloc(NUM_SPHERES, sizeof(struct sphere_s));
	double x = 2.0;
	double y = 2.0;
	int i, j;
	int count = 0;
	for (i = 0; i < 10; i++) {
		for (j = 0; j < 100; j++) {
			spheres[count].id = count;
			spheres[count].pos.x = x;
			spheres[count].pos.y = y;
			spheres[count].pos.z = 10.0;
			spheres[count].vel.x = rand() / (RAND_MAX + 1.0);
			spheres[count].vel.y = rand() / (RAND_MAX + 1.0);
			spheres[count].vel.z = rand() / (RAND_MAX + 1.0);
			spheres[count].mass = 1.0;
			spheres[count].radius = 1.0;
			y = y + 4.0;
			count++;
		}
		y = 2.0;
		x = x + 4.0;
	}
}

// Using hardcoded values for now
void init_grid() {
	grid = calloc(1, sizeof(struct grid_s));
	grid->start.x = 0.0;
	grid->start.y = 0.0;
	grid->start.z = 0.0;
	grid->end.x = 500.0;
	grid->end.y = 500.0;
	grid->end.z = 500.0;
	init_spheres();
}

// For each sphere finds when it will collide with the grid, then finds when it
// will collide with all other spheres.
// The time to the soonest collision is found and all spheres are advanced by 
// this time step.
// Then the colliding sphere(s) have their velocity updated, or if the soonest
// collision is a sphere colliding with a grid then its velocity on the collision
// axis is simply inverted.
double update_grid() {
	// reset
	event_details.time = DBL_MAX;
	event_details.type = COL_NONE;
	event_details.col_axis = AXIS_NONE;
	event_details.sphere_1 = NULL;
	event_details.sphere_2 = NULL;
	int i, j;
	for (i = 0; i < NUM_SPHERES; i++) {
		struct sphere_s *s1 = &(spheres[i]);
		enum AXIS axis;
		double time = find_collision_time_grid(s1, &axis);
		if (time < event_details.time) {
			event_details.type = COL_SPHERE_WITH_GRID;
			event_details.col_axis = axis;
			event_details.time = time;
			event_details.sphere_1 = s1;
			event_details.sphere_2 = NULL;
		}
		for (j = i + 1; j < NUM_SPHERES; j++) {
			struct sphere_s *s2 = &(spheres[j]);
			time = find_collision_time_spheres(s1, s2);
			if (time < event_details.time) {
				event_details.type = COL_TWO_SPHERES;
				event_details.col_axis = AXIS_NONE;
				event_details.time = time;
				event_details.sphere_1 = s1;
				event_details.sphere_2 = s2;
			}
		}
	}
	for (i = 0; i < NUM_SPHERES; i++) {
		struct sphere_s *s = &(spheres[i]);
		update_sphere_position(s, event_details.time);
	}
	if (event_details.type == COL_SPHERE_WITH_GRID) {
		//event_details.col_sphere_1->vel.vals[event_details.col_axis] *= -1.0;
		switch (event_details.col_axis) {
		case X_AXIS:
			event_details.sphere_1->vel.x *= -1;
			break;
		case Y_AXIS:
			event_details.sphere_1->vel.y *= -1;
			break;
		case Z_AXIS:
			event_details.sphere_1->vel.z *= -1;
			break;
		}
	} else if (event_details.type == COL_TWO_SPHERES) {
		apply_bounce_between_spheres(event_details.sphere_1, event_details.sphere_2);
	}
	return event_details.time;
}