#ifndef SPRITE_COMPONENTS_H
#define SPRITE_COMPONENTS_H

#include <SDL3/SDL.h>
#include <flecs.h>

extern ECS_COMPONENT_DECLARE(Sprite);

typedef struct Sprite {
    double x;
    double y;
    int texture;
} Sprite;

void SpriteComponentsImport(ecs_world_t* world);

#endif