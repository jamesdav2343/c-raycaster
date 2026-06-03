#ifndef SPRITE_COMPONENTS_H
#define SPRITE_COMPONENTS_H

#include "types.h"
#include <SDL3/SDL.h>
#include <flecs.h>

extern ECS_COMPONENT_DECLARE(Sprite);

typedef struct Sprite {
    int sprite_id;
} Sprite;

typedef struct {
    Vector2I position;
    Sprite sprite;
} TestEntity;

void SpriteComponentsImport(ecs_world_t* world);

#endif