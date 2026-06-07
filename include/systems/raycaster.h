#ifndef RAYCASTER_SYSTEMS_H
#define RAYCASTER_SYSTEMS_H

#include "components/raycaster.h"
#include "components/transform.h"
#include "general_utils.h"
#include "lighting.h"
#include "map.h"
#include "player.h"
#include "types.h"
#include <SDL3/SDL.h>
#include <flecs.h>

void RaycasterSystemsImport(ecs_world_t* world);

#define TEXTURE_WIDTH_FALLBACK 128
#define TEXTURE_HEIGHT_FALLBACK 128

#define BLACK 0x00000000
#define WHITE 0xFFFFFFFF
#define RED 0xFFFF0000

#endif