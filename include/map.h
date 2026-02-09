#ifndef MAP_H
#define MAP_H
#define rows 10
#define cols 10

#include <SDL3/SDL.h>

extern Uint8 map[rows][cols];

typedef struct MapData
{
    int x_tiles_count;
    int y_tiles_count;
    SDL_FRect *tile;
    Uint8 grid[rows][cols];
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