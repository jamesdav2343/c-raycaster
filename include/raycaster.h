#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL3/SDL.h>
#include "player.h"

typedef struct RayData
{
    // TODO
} RayData;

void draw_rays(SDL_Renderer *renderer, PlayerData *player_data);

#endif
