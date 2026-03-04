#include "player.h"

ecs_entity_t create_player(ecs_world_t *world, float starting_x, float starting_y)
{
    ecs_entity_t player = ecs_new(world);

    ecs_set(world, player, Transform, {{starting_x, starting_y}, TRANSFORM_ZERO.deltaPosition, TRANSFORM_ZERO.rotation});

    return player;
}

void update_player(ecs_world_t *world, ecs_entity_t player)
{
    bool is_updated = false;
    Transform *transform = ecs_get_mut(world, player, Transform);

    if (get_input_direction().y == VECTOR2_UP.y)
    {
        is_updated = true;

        // Updating entity
        transform->position = vector2_add(transform->position, transform->deltaPosition);
    }

    if (get_input_direction().y == VECTOR2_DOWN.y)
    {
        is_updated = true;

        // Updating entity
        transform->position = vector2_subtract(transform->position, transform->deltaPosition);
    }

    if (get_input_direction().x == VECTOR2_LEFT.x)
    {
        is_updated = true;
        rotate_player(world, player, VECTOR2_RIGHT);
    }

    if (get_input_direction().x == VECTOR2_RIGHT.x)
    {
        is_updated = true;
        rotate_player(world, player, VECTOR2_LEFT);
    }

    if (is_updated)
    {
        ecs_modified(world, player, Transform);
        printf("Positions below:\n");
        vector2_print(transform->position);

        printf("Rotation values below:\n");
        printf("%f\n", transform->rotation);

        printf("Delta values below:\n");
        vector2_print(transform->deltaPosition);
    }
}

// void draw_player(SDL_Renderer *renderer, PlayerData *player_data)
// {
//     player_data->rect.x = player_data->position.x;
//     player_data->rect.y = player_data->position.y;

//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
//     SDL_RenderFillRect(renderer, &player_data->rect);
//     SDL_RenderRect(renderer, &player_data->rect);

//     SDL_RenderLine(
//         renderer,
//         player_data->position.x,
//         player_data->position.y,
//         player_data->position.x + player_data->deltaPosition.x * LINE_LENGTH_MULTIPLIER,
//         player_data->position.y + player_data->deltaPosition.y * LINE_LENGTH_MULTIPLIER);
// }

void rotate_player(ecs_world_t *world, ecs_entity_t player, Vector2 direction)
{
    Transform *transform = ecs_get_mut(world, player, Transform);

    if (transform->rotation < 0)
    {
        transform->rotation += 2 * M_PI;
    }

    if (transform->rotation > 2 * M_PI)
    {
        transform->rotation -= 2 * M_PI;
    }

    transform->rotation -= ROTATION_SPEED * direction.x;
    transform->deltaPosition.x = cos(transform->rotation) * ANGLE_MULTIPLIER;
    transform->deltaPosition.y = sin(transform->rotation) * ANGLE_MULTIPLIER;
}