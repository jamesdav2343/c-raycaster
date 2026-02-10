#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL3/SDL.h>
#include "player.h"
#include "map.h"

// Small amount for accuracy with floating point values.
#define EPSILON 0.0001
#define DOF_MAX 4
#define MAP_COORD_MIN 0

typedef struct RayData
{
    // TODO
} RayData;

void draw_rays(SDL_Renderer *renderer, PlayerData *player_data);

#endif
