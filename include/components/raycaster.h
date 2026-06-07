#ifndef RAYCASTER_COMPONENTS_H
#define RAYCASTER_COMPONENTS_H

#include "types.h"
#include <SDL3/SDL.h>
#include <flecs.h>

extern ECS_TAG_DECLARE(Raycaster);
extern ECS_COMPONENT_DECLARE(PixelBuffer);
extern ECS_COMPONENT_DECLARE(ZBuffer);
extern ECS_COMPONENT_DECLARE(Sprite);
extern ECS_COMPONENT_DECLARE(ScreenTexture);

void RaycasterComponentsImport(ecs_world_t* world);

typedef struct PixelBuffer {
    Uint32* pixels;
    int width;
    int height;
    size_t size;
} PixelBuffer;

typedef struct ZBuffer {
    double* buffer;
} ZBuffer;

typedef struct ScreenTexture {
    SDL_Texture* texture;
} ScreenTexture;

typedef struct Wall {
    enum Orientation side_orientation;
    Vector2I wall_position;
} Wall;

// Holds data outputted from the DDA algorithm.
typedef struct Ray {
    Vector2 direction;
    float dist_to_x;
    float dist_to_y;
    float perp_wall_dist;
    Wall wall;
} Ray;

typedef struct Sprite {
    int sprite_id;
} Sprite;

#endif