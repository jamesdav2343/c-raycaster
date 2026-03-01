#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include "transform.h"
#include "controller.h"
#include <math.h>
#include <flecs.h>
#include "ecs.h"

#define ROTATION_SPEED 0.05
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

ecs_entity_t create_player(ecs_world_t *world, float starting_x, float starting_y);

void update_player(PlayerData *player_data);

void draw_player(SDL_Renderer *renderer, PlayerData *player_data);

void rotate_player(PlayerData *player_data, Vector2 direction);

#endif