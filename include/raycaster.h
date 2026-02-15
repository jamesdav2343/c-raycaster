#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL3/SDL.h>
#include "player.h"
#include "map.h"
#include "window.h"

// Small amount for accuracy with floating point values.
#define EPSILON 0.0001
#define DOF_MAX 8
#define MAP_COORD_MIN 0

// Custom raycasting-engine (RCE) math helper (not a part of math.h)
// 3pi/2
#define RCE_3PI_2 3 * M_PI / 2

// One degree in radians
#define RCE_1D 0.0174532925
#define RAY_COUNT 60

// This macro assumes the map has an equal number of rows and columns.
#define MAP_COORD_MAX ROWS
#define LINE_WIDTH 8

typedef struct RayData
{
    // TODO
} RayData;

void draw_rays(SDL_Renderer *renderer, PlayerData *player_data);

void draw_3d_walls(SDL_Renderer *renderer, float distance, float delta_angle, int ray_index, bool hit_horizontal_wall);

int clamp_in_range(int value, int lower_boundary, int upper_boundary);

float distance_between_two_points(float ax, float ay, float bx, float by);

#endif
