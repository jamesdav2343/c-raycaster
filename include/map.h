#ifndef MAP_H
#define MAP_H

#include <SDL3/SDL.h>

// Struct is named _vector2 (in struct namespace),
// global namespace type definition is Vector2
typedef struct _vector2
{
    double x;
    double y;
} Vector2;

void draw_map(SDL_Renderer *renderer, SDL_FRect *rect);

#endif