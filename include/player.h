#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>

typedef struct Vector2
{
    float x;
    float y;
} Vector2;

typedef struct PlayerData
{
    Vector2 *position;
    // Viewpoint angle
    // FOV value
} PlayerData;

void update_player(PlayerData *player_data);

void draw_player(SDL_Renderer *renderer);

#endif