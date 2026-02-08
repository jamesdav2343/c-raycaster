#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include "transform.h"
#include "controller.h"
#include <math.h>

#define ROTATION_SPEED 0.1
#define ANGLE_MULTIPLIER 3
#define LINE_LENGTH_MULTIPLIER 20

typedef struct PlayerData
{
    Vector2 position;
    Vector2 deltaPosition;
    float angle;
    SDL_FRect rect;
    // FOV value
} PlayerData;

void update_player(PlayerData *player_data);

void draw_player(SDL_Renderer *renderer, PlayerData *player_data);

void rotate_player(PlayerData *player_data, Vector2 direction);

#endif