#ifndef SPRITE_H
#define SPRITE_H

#include <flecs.h>
#include <SDL3/SDL.h>

ECS_COMPONENT_DECLARE(Sprite);

typedef struct Sprite
{
    SDL_FRect rect;
} Sprite;

#endif