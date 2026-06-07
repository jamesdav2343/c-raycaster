#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "components/input.h"
#include "systems/input.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <flecs.h>
#include <math.h>

#define ANGLE_MULTIPLIER 3
#define LINE_LENGTH_MULTIPLIER 20

#define MOVEMENT_SPEED 2.5f
#define ROTATION_SPEED 1.5f

#define PLAYER_ENTITY_NAME "_player_"

extern ECS_SYSTEM_DECLARE(PlayerUpdate);

ecs_entity_t Player;

void PlayerModuleImport(ecs_world_t* world);

void PlayerUpdate(ecs_iter_t* it);

void draw_player(SDL_Renderer* renderer, ecs_world_t* world, ecs_entity_t* player);

#endif