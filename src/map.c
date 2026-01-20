#include "map.h"

void draw_map(SDL_Renderer *renderer, SDL_FRect *rect)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
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

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}