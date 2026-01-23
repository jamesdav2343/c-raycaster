#include "player.h"

void update_player(PlayerData *player_data)
{
    printf("updating player\n");
    player_data->position->y++;
}

void draw_player(SDL_Renderer *renderer)
{
    // printf("drawing player\n");
}