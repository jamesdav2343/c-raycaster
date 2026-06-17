#ifndef LIGHTING_H
#define LIGHTING_H

#include "map.h"
#include "types.h"

extern float light_map[ROWS * COLS];
extern float smooth_light_map[ROWS * COLS];

void bake_light_map();

void bake_smooth_light_map();

// Quadrant get_vertices(int pos, float* quadrants);

float get_wall_light_intensity(int x, int y, Vector2 ray_direction, enum Orientation side_orientation);

float get_lighting_floor(float x, float y, int pos);

#endif