#include "player.h"

void update_player(PlayerData *player_data)
{
    Vector2 inputDirection = get_input_direction();
    Vector2 playerPosition = *player_data->position;

    *player_data->position = vector2_add(*player_data->position, inputDirection);

    // Debugging
    vector2_print(player_data->position);
}

void draw_player(SDL_Renderer *renderer)
{
    // printf("drawing player\n");
}