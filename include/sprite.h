#ifndef SPRITE_H
#define SPRITE_H

#include <flecs.h>
#include <SDL3/SDL.h>

extern ECS_COMPONENT_DECLARE(Sprite);

typedef struct Sprite
{
    double x;
    double y;
    int texture;
} Sprite;

void SpriteModuleImport(ecs_world_t *world);

#endif