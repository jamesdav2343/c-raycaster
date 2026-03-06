#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include "transform.h"
#include "controller.h"
#include <math.h>
#include <flecs.h>

#define ROTATION_SPEED 0.05
#define ANGLE_MULTIPLIER 3
#define LINE_LENGTH_MULTIPLIER 20

extern ECS_TAG_DECLARE(Player);
extern ECS_SYSTEM_DECLARE(PlayerUpdate);

extern ecs_entity_t Player;

void PlayerModuleImport(ecs_world_t *world);

void PlayerUpdate(ecs_iter_t *it);

// void draw_player(SDL_Renderer *renderer, PlayerData *player_data);

void rotate_player(ecs_world_t *world, ecs_entity_t player, Position direction);

#endif