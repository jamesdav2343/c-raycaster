#ifndef MAP_H
#define MAP_H
#define mapHeight 10
#define mapWidth 10

#include <SDL3/SDL.h>

// Struct is named _vector2 (in struct namespace),
// global namespace type definition is Vector2
typedef struct Vector2
{
    double x;
    double y;
} Vector2;

typedef struct MapGrid
{
    int x_tiles_count;
    int y_tiles_count;
    SDL_FRect *tile;
} MapGrid;

typedef struct MapData
{
    MapGrid *map_grid;
} MapData;

void draw_map(SDL_Renderer *renderer, MapData *map_data, Uint8 map[mapHeight][mapWidth]);

void print_map(Uint8 *map);

void set_rect_colour(SDL_FRect *rect, Uint8 value);

#endif