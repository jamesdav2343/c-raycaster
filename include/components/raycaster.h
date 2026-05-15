#ifndef RAYCASTER_COMPONENTS_H
#define RAYCASTER_COMPONENTS_H

#include "types.h"
#include <SDL3/SDL.h>
#include <flecs.h>

extern ECS_TAG_DECLARE(Raycaster);
extern ECS_COMPONENT_DECLARE(BufferData);

void RaycasterComponentsImport(ecs_world_t* world);

// Holds data outputted from the DDA algorithm.
struct DdaData {
    enum Orientation side_orientation;
    Vector2 ray_direction;
    float dist_to_x;
    float dist_to_y;
    float perp_wall_dist;
    Vector2I wall_coordinates;
};

typedef struct BufferData {
    Uint32* buffer;
    int width;
    int height;
    size_t size;
} BufferData;

// The pixel buffer that will be rendered.
// extern Uint32 buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

#endif