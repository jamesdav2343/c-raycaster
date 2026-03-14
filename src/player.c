#include "player.h"

ECS_SYSTEM_DECLARE(PlayerUpdate);

void PlayerModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, PlayerModule);

    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, SpriteModule);
    ECS_IMPORT(world, CameraModule);

    ecs_system(
        world, {.entity = ecs_entity(world, {.name = "PlayerUpdate", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
                .query.terms = {{ecs_id(Position)}, {ecs_id(Direction)}, {ecs_id(CameraPlane)}},
                .callback = PlayerUpdate});

    Player = ecs_entity(world, {.add = ecs_ids(EcsPrefab)});

    ecs_set(world, Player, Position, {0.0f, 0.0f});
    ecs_set(world, Player, Direction, {-1.0f, 0.0f});
    ecs_set(world, Player, CameraPlane, {0.0f, 0.66f});

    ecs_set(world, Player, Sprite, {0, 0, 20, 20});
}

void PlayerInput(ecs_iter_t *it)
{
    const bool *key_states = SDL_GetKeyboardState(NULL);
    ecs_entity_t player = ecs_lookup(it->world, PLAYER_ENTITY_NAME);

    for (int i = 0; i < it->count; i++)
    {
        Position *player_position = ecs_get_mut(it->world, player, Position);
        Direction *player_direction = ecs_get_mut(it->world, player, Direction);
        CameraPlane *camera_plane = ecs_get_mut(it->world, player, CameraPlane);

        float move_speed = MOVEMENT_SPEED * it->delta_time; // the constant value is in squares/second
        float rot_speed = ROTATION_SPEED * it->delta_time;  // the constant value is in radians/second
        // move forward if no wall in front of you
        if (key_states[SDL_SCANCODE_W])
        {
            player_position->x += player_direction->x * move_speed;
            player_position->y += player_direction->y * move_speed;
        }
        // move backwards if no wall behind you
        if (key_states[SDL_SCANCODE_S])
        {
            player_position->x -= player_direction->x * move_speed;
            player_position->y -= player_direction->y * move_speed;
        }
        // rotate to the right
        if (key_states[SDL_SCANCODE_D])
        {
            // both camera direction and camera plane must be rotated
            double oldDirX = player_direction->x;
            player_direction->x = player_direction->x * cos(-rot_speed) - player_direction->y * sin(-rot_speed);
            player_direction->y = oldDirX * sin(-rot_speed) + player_direction->y * cos(-rot_speed);
            double old_plane_x = camera_plane->x;
            camera_plane->x = camera_plane->x * cos(-rot_speed) - camera_plane->y * sin(-rot_speed);
            camera_plane->y = old_plane_x * sin(-rot_speed) + camera_plane->y * cos(-rot_speed);
        }
        // rotate to the left
        if (key_states[SDL_SCANCODE_A])
        {
            // both camera direction and camera plane must be rotated
            double oldDirX = player_direction->x;
            player_direction->x = player_direction->x * cos(rot_speed) - player_direction->y * sin(rot_speed);
            player_direction->y = oldDirX * sin(rot_speed) + player_direction->y * cos(rot_speed);
            double old_plane_x = camera_plane->x;
            camera_plane->x = camera_plane->x * cos(rot_speed) - camera_plane->y * sin(rot_speed);
            camera_plane->y = old_plane_x * sin(rot_speed) + camera_plane->y * cos(rot_speed);
        }
    }
}

void PlayerUpdate(ecs_iter_t *it)
{
    for (int i = 0; i < it->count; i++)
    {
        PlayerInput(it);
    }
}

void draw_player(SDL_Renderer *renderer, ecs_world_t *world, ecs_entity_t *player)
{
    // Vector2 *position = ecs_get_mut(world, player, Position);
    // Vector2 *delta_position = ecs_get_mut_pair(world, player, Position, HasDelta);
    // Sprite *sprite = ecs_get_mut(world, Player, Sprite);

    // sprite->rect.x = position->x;
    // sprite->rect.y = position->y;

    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    // SDL_RenderFillRect(renderer, &sprite->rect);
    // SDL_RenderRect(renderer, &sprite->rect);

    // SDL_RenderLine(
    //     renderer,
    //     position->x,
    //     position->y,
    //     position->x + delta_position->x * LINE_LENGTH_MULTIPLIER,
    //     position->y + delta_position->y * LINE_LENGTH_MULTIPLIER);
}