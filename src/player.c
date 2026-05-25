#include "player.h"

ECS_SYSTEM_DECLARE(PlayerUpdate);

void PlayerModuleImport(ecs_world_t* world)
{
    ECS_MODULE(world, PlayerModule);

    ECS_IMPORT(world, TransformComponents);
    ECS_IMPORT(world, SpriteComponents);
    ECS_IMPORT(world, CameraModule);
    ECS_IMPORT(world, InputComponents);

    ecs_system(world,
        { .entity = ecs_entity(world, { .name = "PlayerUpdate", .add = ecs_ids(ecs_dependson(EcsOnUpdate)) }),
            .query.terms = { { ecs_id(Position) }, { ecs_id(Direction) }, { ecs_id(Plane) }, { ecs_id(Controller) } },
            .callback = PlayerUpdate });

    // ECS_SYSTEM_DEFINE(world, PlayerUpdate, EcsOnUpdate, Position, Direction, CameraPlane);

    Player = ecs_entity(world, { .add = ecs_ids(EcsPrefab) });

    ecs_set(world, Player, Position, { 0, 0 });
    ecs_set(world, Player, Direction, { -1, 0 });
    ecs_set(world, Player, Plane, { 0, 0.66 });

    // ecs_set(world, Player, Sprite, {0, 0, 20, 20});
    ecs_set(world, Player, Controller, { 0, 0 });
}

void PlayerUpdate(ecs_iter_t* it)
{
    ecs_entity_t player = ecs_lookup(it->world, PLAYER_ENTITY_NAME);

    Position* player_position = ecs_field(it, Position, 0);
    Direction* camera_direction = ecs_field(it, Direction, 1);
    Plane* camera_plane = ecs_field(it, Plane, 2);
    Controller* controller = ecs_field(it, Controller, 3);

    for (int i = 0; i < it->count; i++) {
        float move_speed = MOVEMENT_SPEED * it->delta_time; // the constant value is in squares/second
        float rot_speed = ROTATION_SPEED * it->delta_time; // the constant value is in radians/second

        // move forward if no wall in front of you
        if (controller->y < 0) {
            player_position[i].x += camera_direction[i].x * move_speed;
            player_position[i].y += camera_direction[i].y * move_speed;
        }
        // move backwards if no wall behind you
        if (controller->y > 0) {
            player_position[i].x -= camera_direction[i].x * move_speed;
            player_position[i].y -= camera_direction[i].y * move_speed;
        }
        // rotate to the right
        if (controller->x > 0) {
            // both camera direction and camera plane must be rotated
            double old_dir_x = camera_direction[i].x;
            camera_direction[i].x = camera_direction[i].x * cos(-rot_speed) - camera_direction[i].y * sin(-rot_speed);
            camera_direction[i].y = old_dir_x * sin(-rot_speed) + camera_direction[i].y * cos(-rot_speed);

            double old_plane_x = camera_plane[i].x;
            camera_plane[i].x = camera_plane[i].x * cos(-rot_speed) - camera_plane[i].y * sin(-rot_speed);
            camera_plane[i].y = old_plane_x * sin(-rot_speed) + camera_plane[i].y * cos(-rot_speed);
        }
        // rotate to the left
        if (controller->x < 0) {
            // both camera direction and camera plane must be rotated
            double old_dir_x = camera_direction[i].x;
            camera_direction[i].x = camera_direction[i].x * cos(rot_speed) - camera_direction[i].y * sin(rot_speed);
            camera_direction[i].y = old_dir_x * sin(rot_speed) + camera_direction[i].y * cos(rot_speed);

            double old_plane_x = camera_plane[i].x;
            camera_plane[i].x = camera_plane[i].x * cos(rot_speed) - camera_plane[i].y * sin(rot_speed);
            camera_plane[i].y = old_plane_x * sin(rot_speed) + camera_plane[i].y * cos(rot_speed);
        }
    }
}

void draw_player(SDL_Renderer* renderer, ecs_world_t* world, ecs_entity_t* player)
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