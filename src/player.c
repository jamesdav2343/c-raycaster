#include "player.h"

ECS_SYSTEM_DECLARE(PlayerUpdate);

void PlayerModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, PlayerModule);

    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, SpriteModule);

    // Macro not working but explicit definition below does
    // ECS_SYSTEM_DEFINE(world, PlayerUpdate, EcsOnUpdate, Position, Rotation);

    ecs_system(
        world, {.entity = ecs_entity(world, {.name = "PlayerUpdate", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
                .query.terms = {{ecs_id(Position)}, {ecs_id(Rotation)}},
                .callback = PlayerUpdate});

    Player = ecs_entity(world, {.name = "Player", .add = ecs_ids(EcsPrefab)});

    HasDelta = ecs_new(world);

    ecs_set(world, Player, Position, {0.0f, 0.0f});

    ecs_add(world, Player, Rotation);
    Rotation *rotation = ecs_get_mut(world, Player, Rotation);
    rotation_set_angle(rotation, 0);
    ecs_modified(world, Player, Rotation);

    ecs_set_pair(world, Player, Position, HasDelta, {0.0f, 0.0f});

    ecs_set(world, Player, Sprite, {0, 0, 20, 20});
}

void rotate_player(ecs_world_t *world, ecs_entity_t player, Position direction)
{
    Position *position = ecs_get_mut(world, player, Position);
    Rotation *rotation = ecs_get_mut(world, player, Rotation);
    Position *delta_position = ecs_get_mut_pair(world, player, Position, HasDelta);

    rotation_add_angle(rotation, -ROTATION_SPEED * direction.x);
    delta_position->x = cos(rotation_get_angle(rotation)) * ANGLE_MULTIPLIER;
    delta_position->y = sin(rotation_get_angle(rotation)) * ANGLE_MULTIPLIER;
}

void PlayerUpdate(ecs_iter_t *it)
{
    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entity = it->entities[i];

        bool is_updated = false;
        Position *position = ecs_get_mut(it->world, entity, Position);
        Position *delta_position = ecs_get_mut_pair(it->world, entity, Position, HasDelta);

        if (get_input_direction().y == VECTOR2_UP.y)
        {
            is_updated = true;
            position->x += delta_position->x;
            position->y += delta_position->y;
        }

        if (get_input_direction().y == VECTOR2_DOWN.y)
        {
            is_updated = true;
            position->x -= delta_position->x;
            position->y -= delta_position->y;
        }

        if (get_input_direction().x == VECTOR2_LEFT.x)
        {
            is_updated = true;
            rotate_player(it->world, entity, VECTOR2_RIGHT);
        }

        if (get_input_direction().x == VECTOR2_RIGHT.x)
        {
            is_updated = true;
            rotate_player(it->world, entity, VECTOR2_LEFT);
        }

        if (is_updated)
            ecs_modified(it->world, entity, Position);
    }
}

void draw_player(SDL_Renderer *renderer, ecs_world_t *world, ecs_entity_t *player)
{
    Position *position = ecs_get_mut(world, player, Position);
    Position *delta_position = ecs_get_mut_pair(world, player, Position, HasDelta);
    Sprite *sprite = ecs_get_mut(world, Player, Sprite);

    sprite->rect.x = position->x;
    sprite->rect.y = position->y;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &sprite->rect);
    SDL_RenderRect(renderer, &sprite->rect);

    SDL_RenderLine(
        renderer,
        position->x,
        position->y,
        position->x + delta_position->x * LINE_LENGTH_MULTIPLIER,
        position->y + delta_position->y * LINE_LENGTH_MULTIPLIER);
}