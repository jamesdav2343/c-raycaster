#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <math.h>
#include <flecs.h>
#include "transform.h"
#include "controller.h"

#define ROTATION_SPEED 0.05
#define ANGLE_MULTIPLIER 3
#define LINE_LENGTH_MULTIPLIER 20

extern ECS_SYSTEM_DECLARE(PlayerUpdate);

ecs_entity_t Player;
ecs_entity_t HasDelta;

void PlayerModuleImport(ecs_world_t *world);

void PlayerUpdate(ecs_iter_t *it);

// void draw_player(SDL_Renderer *renderer, PlayerData *player_data);

#endif