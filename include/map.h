#ifndef MAP_H
#define MAP_H

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
} MapGrid;

typedef struct MapData
{
    MapGrid *map_grid;
    // a field for the map layout, i.e. where the walls are, so will be 2d array
} MapData;

void draw_map(SDL_Renderer *renderer, SDL_FRect *rect, MapGrid *map_data);

#endif