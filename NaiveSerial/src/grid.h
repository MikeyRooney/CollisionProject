#pragma once

#include "collision.h"
#include "sphere.h"
#include "vector_3.h"

enum AXIS {
	X_AXIS = 0,
	Y_AXIS = 1,
	Z_AXIS = 2,
	AXIS_NONE = -1
};

// This is the area in which the collisions take place. 
// Imagine the grid is a 3D box with all collisions taking place inside it.
// Normally x_start, y_start and z_start will all be 0, but the code is 
// built to work with other positive values.
// Note that all values here MUST be positive. 
struct grid_s {
	union vector_3d start;
	union vector_3d end;
};

struct grid_s *grid; // The grid used by the simulation.

struct event_s {
	double time;  // When the event happens
	enum collision_type type; // What the next event is.
	struct sphere_s *sphere_1; // Sphere that hits the grid or is the first sphere in a sphere on sphere collision.
	struct sphere_s *sphere_2; // Second sphere in a sphere on sphere collision, otherwise NULL
	enum AXIS col_axis; // If the event is a sphere bouncing off the grid record the axis
};

struct event_s event_details;

// All spheres in the grid.
// Hardcoded for testing
//#define NUM_SPHERES 5
uint64_t NUM_SPHERES;
struct sphere_s *spheres;

void init_grid();
double update_grid();