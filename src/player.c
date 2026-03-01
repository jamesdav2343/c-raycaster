#include "player.h"

ecs_entity_t create_player(ecs_world_t *world, float starting_x, float starting_y)
{
    ecs_entity_t player = ecs_new(world);

    ecs_set(world, player, Vector2, {starting_x, starting_y});

    return player;
}

void update_player(PlayerData *player_data)
{
    if (get_input_direction().y == VECTOR2_UP.y)
    {
        player_data->position = vector2_add(player_data->position, player_data->deltaPosition);
    }

    if (get_input_direction().y == VECTOR2_DOWN.y)
    {
        player_data->position = vector2_subtract(player_data->position, player_data->deltaPosition);
    }

    if (get_input_direction().x == VECTOR2_LEFT.x)
    {
        rotate_player(player_data, VECTOR2_RIGHT);
    }

    if (get_input_direction().x == VECTOR2_RIGHT.x)
    {
        rotate_player(player_data, VECTOR2_LEFT);
    }
}

void draw_player(SDL_Renderer *renderer, PlayerData *player_data)
{
    player_data->rect.x = player_data->position.x;
    player_data->rect.y = player_data->position.y;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &player_data->rect);
    SDL_RenderRect(renderer, &player_data->rect);

    SDL_RenderLine(
        renderer,
        player_data->position.x,
        player_data->position.y,
        player_data->position.x + player_data->deltaPosition.x * LINE_LENGTH_MULTIPLIER,
        player_data->position.y + player_data->deltaPosition.y * LINE_LENGTH_MULTIPLIER);
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
    player_data->deltaPosition.x = cos(player_data->angle) * ANGLE_MULTIPLIER;
    player_data->deltaPosition.y = sin(player_data->angle) * ANGLE_MULTIPLIER;
}