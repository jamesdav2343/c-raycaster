#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL3/SDL.h>
#include "player.h"
#include "map.h"

// Small amount for accuracy with floating point values.
#define EPSILON 0.0001
#define DOF_MAX 4
#define MAP_COORD_MIN 0

// Custom raycasting-engine (RCE) math helper (not a part of math.h)
// 3pi/2
#define RCE_3PI_2 3 * M_PI / 2

// This macro assumes the map has an equal number of rows and columns.
#define MAP_COORD_MAX ROWS

typedef struct RayData
{
    // TODO
} RayData;

void draw_rays(SDL_Renderer *renderer, PlayerData *player_data);

int clamp_in_range(int value, int lower_boundary, int upper_boundary);

#endif
