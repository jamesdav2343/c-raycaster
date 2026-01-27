#include "player.h"

void update_player(PlayerData *player_data)
{
    *player_data->position = vector2_add(*player_data->position, get_input_direction());

    // Debugging
    // vector2_print(player_data->position);
}

void draw_player(SDL_Renderer *renderer, PlayerData *player_data)
{
    // printf("drawing player\n");
    SDL_FRect tile = {player_data->position->x, player_data->position->y, 10, 10};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &tile);
    SDL_RenderRect(renderer, &tile);
}