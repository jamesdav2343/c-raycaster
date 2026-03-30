#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL3/SDL.h>
#include <flecs.h>
#include "player.h"
#include "map.h"
#include "window.h"
#include "game_manager.h"

extern ECS_SYSTEM_DECLARE(RaycasterUpdate);
extern ECS_SYSTEM_DECLARE(RaycasterDraw);
extern ECS_SYSTEM_DECLARE(RaycasterDestroy);
extern ECS_TAG_DECLARE(Raycaster);
extern ECS_COMPONENT_DECLARE(BufferData);

void RaycasterModuleImport(ecs_world_t *world);

// Small amount for accuracy with floating point values.
#define EPSILON 0.0001
#define DOF_MAX 8
#define MAP_COORD_MIN 0

// Custom raycasting-engine (RCE) math helper (not a part of math.h)
// 3pi/2
#define RCE_3PI_2 3 * M_PI / 2

// One degree in radians
#define RCE_1D 0.0174532925
#define RAY_COUNT 60

// This macro assumes the map has an equal number of rows and columns.
#define MAP_COORD_MAX ROWS
#define LINE_WIDTH 1

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

extern const int DRAW_START_MIN;
extern const int DRAW_END_MAX;

SDL_Texture *pixels;
Uint32 *textures[8];
void *texture_pixels;
int texture_pitch;

enum Orientation
{
    HORIZONTAL = 0,
    VERTICAL = 1
};

// Holds data outputted from the DDA algorithm.
struct DdaData
{
    enum Orientation side_orientation;
    Vector2 ray_direction;
    float dist_to_x;
    float dist_to_y;
    float perp_wall_dist;
};

typedef struct BufferData
{
    Uint32 *buffer;
    int width;
    int height;
    size_t size;
} BufferData;

// The pixel buffer that will be rendered.
// extern Uint32 buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

/*
Digital differential analyser algorithm
Great explanation of the algorithm here:
https://aaaa.sh/creatures/dda-algorithm-interactive/
*/
void dda(const Position *position, const Direction *direction, const Plane *plane, int screen_x, int screen_width, int screen_height, struct DdaData *output_dda_data);

void write_to_buffer(const Position *position, const Direction *direction, const Plane *plane, BufferData *buffer_data);

void RaycasterUpdate(ecs_iter_t *it);

void RaycasterDestroy(ecs_iter_t *it);

void RaycasterDraw(ecs_iter_t *it);

#endif
