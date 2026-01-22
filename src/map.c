#include "map.h"

void draw_map(SDL_Renderer *renderer, MapData *map_data, Uint8 map[mapHeight][mapWidth])
{
    SDL_FRect *tile = map_data->map_grid->tile;
    SDL_Color tile_colour;

    for (int i = 0; i < map_data->map_grid->y_tiles_count; i++)
    {
        for (int j = 0; j < map_data->map_grid->x_tiles_count; j++)
        {
            tile_colour = get_tile_colour(map[i][j]);
            SDL_SetRenderDrawColor(renderer, tile_colour.r, tile_colour.g, tile_colour.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, tile);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderRect(renderer, tile);

            // Move to next column
            tile->x = tile->x + tile->w;
        }

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

SDL_Color get_tile_colour(Uint8 colourValue)
{
    SDL_Color colour;

    switch (colourValue)
    {
    case WHITE:
        colour = (SDL_Color){255, 255, 255, 255};
        break;
    case BLUE:
        colour = (SDL_Color){0, 0, 255, 255};
        break;
    case ORANGE:
        colour = (SDL_Color){255, 165, 0, 255};
        break;
    default:
        colour = (SDL_Color){0, 0, 0, 255};
        break;
    }

    return colour;
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