#ifndef PLAYER_H
#define PLAYER_H

#include "components/input.h"
#include "components/transform.h"
#include "systems/input.h"
#include "types.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <flecs.h>
#include <math.h>

#define ANGLE_MULTIPLIER 3
#define LINE_LENGTH_MULTIPLIER 20

#define MOVEMENT_SPEED 2.5f
#define ROTATION_SPEED 1.5f
#define CAMERA_VERTICAL_MOVEMENT_SPEED 5.0
#define PITCH_MAX 128
#define PITCH_MIN -128

#define PLAYER_ENTITY_NAME "_player_"

extern ECS_SYSTEM_DECLARE(PlayerUpdate);
extern ECS_COMPONENT_DECLARE(Camera);
extern ECS_COMPONENT_DECLARE(Pitch);

// The camera plane.
typedef Vector2 Camera;

typedef struct Pitch {
    double value;
} Pitch;

ecs_entity_t Player;

void PlayerModuleImport(ecs_world_t* world);

void PlayerUpdate(ecs_iter_t* it);

void draw_player(SDL_Renderer* renderer, ecs_world_t* world, ecs_entity_t* player);

#endif