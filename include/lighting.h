#ifndef LIGHTING_H
#define LIGHTING_H

#include "map.h"
#include "types.h"

#define SMOOTH_LM_LEN (ROWS + 1) * (COLS + 1)
#define ALL_VERTICES_WIDTH (COLS + 1)
#define ALL_VERTICES_HEIGHT (ROWS + 1)

extern float light_map[ROWS * COLS];
extern float all_vertices[ALL_VERTICES_WIDTH * ALL_VERTICES_HEIGHT];

void bake_light_map();

void bake_smooth_light_map();

// Quadrant get_vertices(int pos, float* quadrants);

float get_wall_light_intensity(int x, int y, Vector2 ray_direction, enum Orientation orientation);

float get_lighting_floor(float x, float y, int pos);

float get_lighting_wall(float x, float y, int pos, enum Orientation orientation);

#endif