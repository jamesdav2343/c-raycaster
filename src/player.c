#include "player.h"

void update_player(PlayerData *player_data)
{
    *player_data->position = vector2_add(*player_data->position, get_input_direction());

    // Debugging
    vector2_print(player_data->position);
}

void draw_player(SDL_Renderer *renderer)
{
    // printf("drawing player\n");
}