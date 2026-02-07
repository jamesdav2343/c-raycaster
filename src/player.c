#include "player.h"

void update_player(PlayerData *player_data)
{
    if (vector2_equals(get_input_direction(), VECTOR2_UP))
    {
        *player_data->position = vector2_add(*player_data->position, *player_data->deltaPosition);
    }

    if (vector2_equals(get_input_direction(), VECTOR2_DOWN))
    {
        *player_data->position = vector2_subtract(*player_data->position, *player_data->deltaPosition);
    }

    if (vector2_equals(get_input_direction(), VECTOR2_LEFT))
    {
        rotate_player(player_data, VECTOR2_LEFT);
    }

    if (vector2_equals(get_input_direction(), VECTOR2_RIGHT))
    {
        rotate_player(player_data, VECTOR2_RIGHT);
    }
}

void draw_player(SDL_Renderer *renderer, PlayerData *player_data)
{
    SDL_FRect tile = {player_data->position->x, player_data->position->y, 10, 10};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &tile);
    SDL_RenderRect(renderer, &tile);

    SDL_RenderLine(
        renderer,
        player_data->position->x,
        player_data->position->y,
        player_data->position->x + player_data->deltaPosition->x * ANGLE_MULTIPLIER,
        player_data->position->y + player_data->deltaPosition->y * ANGLE_MULTIPLIER);
}

void rotate_player(PlayerData *player_data, Vector2 direction)
{
    if (player_data->angle < 0)
    {
        player_data->angle += 2 * M_PI;
    }

    if (player_data->angle > 2 * M_PI)
    {
        player_data->angle -= 2 * M_PI;
    }

    player_data->angle -= ROTATION_SPEED * direction.x;
    player_data->deltaPosition->x = cos(player_data->angle) * ANGLE_MULTIPLIER;
    player_data->deltaPosition->y = sin(player_data->angle) * ANGLE_MULTIPLIER;
}