#include "map.h"

void draw_map(SDL_Renderer *renderer, MapData *map_data, Uint8 map[mapHeight][mapWidth])
{
    // printf("%d : %d\n", map_data->x_tiles_count, map_data->y_tiles_count);

    SDL_FRect *tile = map_data->map_grid->tile;

    for (int i = 0; i < map_data->map_grid->y_tiles_count; i++)
    {
        for (int j = 0; j < map_data->map_grid->x_tiles_count; j++)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            // set_rect_colour(tile, map[i][j]);
            SDL_RenderRect(renderer, map_data->map_grid->tile);

            // printf("%d ", map[i][j]);

            // Move to next column
            tile->x = tile->x + tile->w;
        }
        // printf("\n");

        // Move to start of next row
        tile->x = 0;
        tile->y = tile->y + tile->h;
    }

    // Reset
    tile->y = 0;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

void set_rect_colour(SDL_FRect *rect, Uint8 colour)
{
    // SDL_RenderFillRect(renderer, rect);
}

// Prints array using pointer arithemtic
void print_map(Uint8 *map)
{
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            printf("%d ", *(map));
            map++;
        }
        printf("\n");
    }
}