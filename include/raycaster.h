#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL3/SDL.h>
#include <flecs.h>
#include "player.h"
#include "map.h"
#include "window.h"
#include "game_manager.h"

extern ECS_SYSTEM_DECLARE(RaycasterUpdate);
extern ECS_SYSTEM_DECLARE(RaycasterDestroy);
extern ECS_TAG_DECLARE(Raycaster);

void RaycasterModuleImport(ecs_world_t *world);

// Small amount for accuracy with floating point values.
#define EPSILON 0.0001
#define DOF_MAX 8
#define MAP_COORD_MIN 0

// Custom raycasting-engine (RCE) math helper (not a part of math.h)
// 3pi/2
#define RCE_3PI_2 3 * M_PI / 2

// One degree in radians
#define RCE_1D 0.0174532925
#define RAY_COUNT 60

// This macro assumes the map has an equal number of rows and columns.
#define MAP_COORD_MAX ROWS
#define LINE_WIDTH 1

void draw_rays_dda(SDL_Renderer *renderer, ecs_world_t *world, ecs_entity_t player, float delta_time);

void RaycasterUpdate(ecs_iter_t *it);

void RaycasterDestroy(ecs_iter_t *it);

#endif
