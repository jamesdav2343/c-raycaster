#ifndef SPRITE_SYSTEMS_H
#define SPRITE_SYSTEMS_H

#include "components/sprite.h"
#include <flecs.h>

extern ECS_SYSTEM_DECLARE(SpriteDraw);

void SpriteSystemsImport(ecs_world_t* world);

void SpriteDraw(ecs_iter_t* it);

#endif