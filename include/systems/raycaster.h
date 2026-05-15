#ifndef RAYCASTER_SYSTEMS_H
#define RAYCASTER_SYSTEMS_H

#include "components/raycaster.h"
#include "components/transform.h"
#include "game_manager.h"
#include "general_utils.h"
#include "lighting.h"
#include "map.h"
#include "player.h"
#include "types.h"
#include "window.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <flecs.h>

extern ECS_SYSTEM_DECLARE(RaycasterUpdate);
// extern ECS_SYSTEM_DECLARE(RaycasterDestroy);
extern ECS_SYSTEM_DECLARE(RaycasterDraw);

void RaycasterSystemsImport(ecs_world_t* world);

#define TEXTURE_WIDTH 128
#define TEXTURE_HEIGHT 128

#define SPRITE_TEXTURE_WIDTH 64
#define SPRITE_TEXTURE_HEIGHT 64

#define BLACK 0x00000000
#define MAX_SHADOW 0.9f
#define MIN_SHADOW 0.0f

#define SHADOW_LEVEL 1 // 1 (minumum) to whatever

extern const int DRAW_START_MIN;
extern const int DRAW_END_MAX;

/*
Casts a ray.
Uses the digital differential analyser algorithm.
Great explanation of the algorithm here:
https://aaaa.sh/creatures/dda-algorithm-interactive/
*/
void dda(const Position* position, const Direction* direction, const Plane* plane, int screen_x, int screen_width,
    int screen_height, struct DdaData* output_dda_data);

void clear_buffer(BufferData* buffer_data);

void write_to_buffer(const Position* position, const Direction* direction, const Plane* plane, BufferData* buffer_data);

void blit_buffer_to_texture(SDL_Texture* dest_pixels_texture, BufferData* src_buffer_data);

/*
Writes a vertical wall strip to the pixel buffer.
*/
void write_vertical_wall_strip(
    struct DdaData* dda_data, const Position* position, int current_x, BufferData* dest_buffer_data);

/*
Writes the horizontal wall and ceiling strips to the pixel buffer.
*/
void write_floor_and_celing(const Position* position, const Direction* direction, const Plane* plane,
    BufferData* dest_buffer_data, int screen_width, int screen_height);

void RaycasterUpdate(ecs_iter_t* it);

void RaycasterDraw(ecs_iter_t* it);

#endif