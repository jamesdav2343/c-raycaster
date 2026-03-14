#include "raycaster.h"

ECS_SYSTEM_DECLARE(RaycasterUpdate);
ECS_SYSTEM_DECLARE(RaycasterDestroy);
ECS_TAG_DECLARE(Raycaster);

void RaycasterModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, RaycasterModule);

    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, SpriteModule);
    ECS_IMPORT(world, CameraModule);

    ECS_TAG_DEFINE(world, Raycaster);
    ECS_SYSTEM_DEFINE(world, RaycasterUpdate, EcsOnUpdate, Raycaster);
    ECS_SYSTEM_DEFINE(world, RaycasterDestroy, EcsOnDelete, Raycaster);
}

void RaycasterUpdate(ecs_iter_t *it)
{
    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t player = ecs_lookup(it->world, PLAYER_ENTITY_NAME);
        SDL_Renderer *renderer = ecs_get(it->world, ecs_id(Renderer), Renderer)->ptr;

        draw_rays_dda(renderer, it->world, player);

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }
}

void RaycasterDestroy(ecs_iter_t *it)
{
    SDL_Renderer *renderer = ecs_get(it->world, ecs_id(Renderer), Renderer)->ptr;

    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    printf("Game closed.\n");
}

void ver_line(SDL_Renderer *renderer, int x, int start_y, int end_y, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderLine(renderer, x, start_y, x, end_y);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

/*
Notes on this function:
Lodev's implementation of raycaster DDA algorithm
*/
void draw_rays_dda(SDL_Renderer *renderer, ecs_world_t *world, ecs_entity_t player)
{
    Position *player_position = ecs_get_mut(world, player, Position);
    Direction *player_direction = ecs_get_mut(world, player, Direction);
    Plane *camera_plane = ecs_get_mut(world, player, Plane);

    int w = SCREEN_WIDTH;
    int h = SCREEN_HEIGHT;

    for (int x = 0; x < w; x++)
    {
        double camera_x = 2 * x / (double)w - 1;
        Vector2 ray_dir = {player_direction->x + camera_plane->x * camera_x, player_direction->y + camera_plane->y * camera_x};

        // Which box of the map we're in
        int map_x = (int)floorf(player_position->x);
        int map_y = (int)floorf(player_position->y);

        // Length of ray from current position to next x or y-side
        Vector2 side_dist = {0, 0};

        Vector2 delta_dist = {
            (ray_dir.x == 0) ? 1e30 : fabs(1 / ray_dir.x),
            (ray_dir.y == 0) ? 1e30 : fabs(1 / ray_dir.y)};

        double perp_wall_dist;

        int step_x;
        int step_y;

        int hit = 0;
        int side;

        if (ray_dir.x < 0)
        {
            step_x = -1;
            side_dist.x = (player_position->x - map_x) * delta_dist.x;
        }
        else
        {
            step_x = 1;
            side_dist.x = (map_x + 1.0 - player_position->x) * delta_dist.x;
        }

        if (ray_dir.y < 0)
        {
            step_y = -1;
            side_dist.y = (player_position->y - map_y) * delta_dist.y;
        }
        else
        {
            step_y = 1;
            side_dist.y = (map_y + 1.0 - player_position->y) * delta_dist.y;
        }

        // Perform DDA
        while (hit == 0)
        {
            // jump to next map square, either in x-direction, or in y-direction
            if (side_dist.x < side_dist.y)
            {
                side_dist.x += delta_dist.x;
                map_x += step_x;
                side = 0;
            }
            else
            {
                side_dist.y += delta_dist.y;
                map_y += step_y;
                side = 1;
            }

            // Check if ray has hit a wall
            if (map[map_x][map_y] > 0)
                hit = 1;
        }

        if (side == 0)
        {
            perp_wall_dist = side_dist.x - delta_dist.x;
        }
        else
        {
            perp_wall_dist = side_dist.y - delta_dist.y;
        }

        int line_height = (int)(h / perp_wall_dist);

        int draw_start = -line_height / 2 + h / 2;

        if (draw_start < 0)
            draw_start = 0;

        int draw_end = line_height / 2 + h / 2;

        if (draw_end >= h)
            draw_end = h - 1;

        // Choose wall color
        SDL_Color color;
        switch (map[map_x][map_y])
        {
        case 1:
            color = (SDL_Color){255, 0, 0, SDL_ALPHA_OPAQUE};
            break; // red
        case 2:
            color = (SDL_Color){0, 255, 0, SDL_ALPHA_OPAQUE};
            break; // green
        case 3:
            color = (SDL_Color){0, 0, 255, SDL_ALPHA_OPAQUE};
            break; // blue
        case 4:
            color = (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE};
            break; // white
        default:
            color = (SDL_Color){255, 222, 33, SDL_ALPHA_OPAQUE};
            break; // yellow
        }

        // draw the pixels of the stripe as a vertical line
        ver_line(renderer, x, draw_start, draw_end, color);
    }

}