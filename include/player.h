#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>
#include "transform.h"
#include "controller.h"

typedef struct PlayerData
{
    Vector2 *position;
    float dx;
    float dy;
    float angle;
    // Viewpoint angle
    // FOV value
} PlayerData;

void update_player(PlayerData *player_data);

void draw_player(SDL_Renderer *renderer, PlayerData *player_data);

#endif