#pragma once

#include <stdint.h>

#include "vector_3.h"

// Velocity is given as metres per second
// Position is given in metres and is the center of the sphere.
// For now assuming all spheres have the same radius
struct sphere_s {
	uint64_t id; // global id
	uint64_t sector_id; // id within the current sector's array of spheres
	union vector_3d vel;
	union vector_3d pos;
	double radius;
	double mass;
};

void update_sphere_position(struct sphere_s *s, double t);