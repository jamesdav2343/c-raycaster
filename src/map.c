#include "map.h"

void draw_map(SDL_Renderer *renderer, SDL_FRect *rect, MapGrid *map_data)
{
    // printf("%d : %d\n", map_data->x_tiles_count, map_data->y_tiles_count);

    for (int i = 0; i < map_data->y_tiles_count; i++)
    {
        for (int j = 0; j < map_data->x_tiles_count; j++)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            // SDL_RenderFillRect(renderer, rect);
            SDL_RenderRect(renderer, rect);

            // Move to next column
            rect->x = rect->x + rect->w;
        }
        // Move to start of next row
        rect->x = 0;
        rect->y = rect->y + rect->h;
    }

    // Reset
    rect->y = 0;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}