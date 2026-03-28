#ifndef MAP_H
#define MAP_H

#define ROWS 24
#define COLS 24
#define TILE_PIXEL_COUNT 64

#include <SDL3/SDL.h>

extern Uint8 world_map[ROWS][COLS];

typedef struct MapData
{
    int x_tiles_count;
    int y_tiles_count;
    SDL_FRect *tile;
    Uint8 grid[ROWS][COLS];
} MapData;

enum rect_colour
{
    WHITE = 0,
    BLUE = 1,
    ORANGE = 2
};

void draw_map(SDL_Renderer *renderer, MapData *map_data);

void print_map(Uint8 *map);

SDL_Color get_tile_colour(Uint8 value);

#endif