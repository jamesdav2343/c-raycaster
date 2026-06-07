#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include "data_structures/ht.h"
#include <SDL3/SDL.h>
#include <cjson/cJSON.h>

#define ALPHA_OPAQUE_HEX 0xFF000000
#define BUFFER_MAX 1024

/**
 * interpolate 2 RGB colors
 * @param color1    integer containing color as 0x00RRGGBB
 * @param color2    integer containing color as 0x00RRGGBB
 * @param fraction  how much interpolation (0..1)
 * - 0: full color 1
 * - 1: full color 2
 * @return the new color after interpolation
 *
 * Credit to @Carlos Barcellos on Stack Overflow
 * https://stackoverflow.com/a/21010385
 */
Uint32 interpolate(int color1, int color2, float fraction);

/**
 * loads raycaster config from a .ini file
 */
cJSON* load_config_json(const char* path);

SDL_Surface* load_formatted_img_surface(const char* img_path, SDL_PixelFormat format);

ht* create_textures_from_config(ht* config);

int compare_sprites(const void* a, const void* b);

void sort_sprites(int* order, double* distance, int amount);

void pretty_print_grid(Uint8* grid, int row_length, int max_gap);

#endif