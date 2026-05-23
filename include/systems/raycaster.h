#ifndef RAYCASTER_SYSTEMS_H
#define RAYCASTER_SYSTEMS_H

#include "components/raycaster.h"
#include "components/transform.h"
#include "general_utils.h"
#include "lighting.h"
#include "map.h"
#include "player.h"
#include "types.h"
#include "window.h"
#include <SDL3/SDL.h>
#include <flecs.h>

extern ECS_SYSTEM_DECLARE(RaycasterUpdate);
// extern ECS_SYSTEM_DECLARE(RaycasterDestroy);
extern ECS_SYSTEM_DECLARE(RaycasterDraw);

void RaycasterSystemsImport(ecs_world_t* world);

#define TEXTURE_WIDTH 128
#define TEXTURE_HEIGHT 128

#define SPRITE_TEXTURE_WIDTH 64
#define SPRITE_TEXTURE_HEIGHT 64

#define BLACK 0x00000000

void RaycasterUpdate(ecs_iter_t* it);

void RaycasterDraw(ecs_iter_t* it);

void raycaster_cleanup(ecs_world_t* world, void* ctx);

#endif