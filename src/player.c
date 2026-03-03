#include "player.h"

ecs_entity_t create_player(ecs_world_t *world, float starting_x, float starting_y)
{
    ecs_entity_t player = ecs_new(world);

    ecs_set(world, player, Transform, {{starting_x, starting_y}, DEFAULT_TRANSFORM.deltaPosition, DEFAULT_TRANSFORM.rotation});

    return player;
}

void update_player(ecs_world_t *world, PlayerData *player_data, ecs_entity_t player)
{
    bool is_updated = false;
    Transform *transform = ecs_get_mut(world, player, Transform);

    if (get_input_direction().y == VECTOR2_UP.y)
    {
        is_updated = true;
        player_data->position = vector2_add(player_data->position, player_data->deltaPosition);

        // Updating entity
        transform->position = vector2_add(transform->position, player_data->deltaPosition);
    }

    if (get_input_direction().y == VECTOR2_DOWN.y)
    {
        is_updated = true;
        player_data->position = vector2_subtract(player_data->position, player_data->deltaPosition);

        // Updating entity
        transform->position = vector2_subtract(transform->position, player_data->deltaPosition);
    }

    if (get_input_direction().x == VECTOR2_LEFT.x)
    {
        is_updated = true;
        rotate_player(player_data, VECTOR2_RIGHT);
    }

    if (get_input_direction().x == VECTOR2_RIGHT.x)
    {
        is_updated = true;
        rotate_player(player_data, VECTOR2_LEFT);
    }

    if (is_updated)
    {
        ecs_modified(world, player, Transform);
        vector2_print(&player_data->position);
        vector2_print(&transform->position);
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