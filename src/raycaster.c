#include "raycaster.h"

// Clamps a value between a lower and upper boundary.
int clamp_in_range(int value, int lower_boundary, int upper_boundary)
{
    int max = fmax(value, lower_boundary);
    return (int)fmin(max, upper_boundary);
}

float distance_between_two_points(float ax, float ay, float bx, float by)
{
    return sqrtf((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

/*
Notes on this function:

    - The map position index values are computed by dividing the coordinate value
    by the TILE_PIXEL_COUNT value, which here is 64. Be aware that if the
    TILE_PIXEL_COUNT value is changed then all bitshift operations
    ( >> 6, which are just divisions by 64) here will have to be changed also to work
    with the new value.

*/
void draw_rays(SDL_Renderer *renderer, ecs_world_t *world, ecs_entity_t player)
{
    Position *player_position = ecs_get_mut(world, player, Position);
    Rotation *player_rotation = ecs_get_mut(world, player, Rotation);

    int map_x, map_y, depth_of_field;
    float ray_x, ray_y, x_offset, y_offset;

    Rotation *ray_rotation = create_rotation(rotation_get_angle(player_rotation));
    float ray_angle = rotation_add_angle(ray_rotation, -RCE_1D * 30);

    for (int ray_i = 0; ray_i < RAY_COUNT; ray_i++)
    {
        // Check horizontal lines
        depth_of_field = 0;
        float aTan = -1 / tan(ray_angle);

        // Looking up
        if (ray_angle > M_PI)
        {
            ray_y = (((int)player_position->y >> 6) << 6) - EPSILON;
            ray_x = (player_position->y - ray_y) * aTan + player_position->x;
            y_offset = -TILE_PIXEL_COUNT;
            x_offset = -y_offset * aTan;
        }
        // Looking down
        else if (ray_angle < M_PI)
        {
            ray_y = (((int)player_position->y >> 6) << 6) + TILE_PIXEL_COUNT;
            ray_x = (player_position->y - ray_y) * aTan + player_position->x;
            y_offset = TILE_PIXEL_COUNT;
            x_offset = -y_offset * aTan;
        }
        // Looking straight left or right
        else
        {
            ray_x = player_position->x;
            ray_y = player_position->y;
            depth_of_field = DOF_MAX;
        }

        while (depth_of_field < DOF_MAX)
        {
            map_x = (int)ray_x >> 6;
            map_y = (int)ray_y >> 6;

            map_x = clamp_in_range(map_x, MAP_COORD_MIN, MAP_COORD_MAX);
            map_y = clamp_in_range(map_y, MAP_COORD_MIN, MAP_COORD_MAX);

            if (map[map_y][map_x] >= 1)
            {
                depth_of_field = DOF_MAX;
            }
            else
            {
                ray_x += x_offset;
                ray_y += y_offset;
                depth_of_field += 1;
            }
        }

        float horizontal_x = ray_x;
        float horizontal_y = ray_y;

        // Check vertical lines
        depth_of_field = 0;
        float nTan = -tan(ray_angle);

        // Looking left
        if (ray_angle > M_PI_2 && ray_angle < RCE_3PI_2)
        {
            ray_x = (((int)player_position->x >> 6) << 6) - EPSILON;
            ray_y = (player_position->x - ray_x) * nTan + player_position->y;
            x_offset = -TILE_PIXEL_COUNT;
            y_offset = -x_offset * nTan;
        }
        // Looking right
        else if (ray_angle < M_PI_2 || ray_angle > RCE_3PI_2)
        {
            ray_x = (((int)player_position->x >> 6) << 6) + TILE_PIXEL_COUNT;
            ray_y = (player_position->x - ray_x) * nTan + player_position->y;
            x_offset = TILE_PIXEL_COUNT;
            y_offset = -x_offset * nTan;
        }
        // Looking straight up or down
        else
        {
            ray_x = player_position->x;
            ray_y = player_position->y;
            depth_of_field = DOF_MAX;
        }

        while (depth_of_field < DOF_MAX)
        {
            map_x = (int)ray_x >> 6;
            map_y = (int)ray_y >> 6;

            map_x = clamp_in_range(map_x, MAP_COORD_MIN, MAP_COORD_MAX);
            map_y = clamp_in_range(map_y, MAP_COORD_MIN, MAP_COORD_MAX);

            if (map[map_y][map_x] >= 1)
            {
                depth_of_field = DOF_MAX;
            }
            else
            {
                ray_x += x_offset;
                ray_y += y_offset;
                depth_of_field += 1;
            }
        }

        float horizontal_distance = distance_between_two_points(horizontal_x, horizontal_y, player_position->x, player_position->y);
        float vertical_distance = distance_between_two_points(ray_x, ray_y, player_position->x, player_position->y);
        float shortest_distance;
        bool hit_horizontal_wall = false;

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

        if (horizontal_distance <= vertical_distance)
        {
            shortest_distance = horizontal_distance;
            ray_x = horizontal_x;
            ray_y = horizontal_y;
            hit_horizontal_wall = true;
        }
        else
        {
            shortest_distance = vertical_distance;
        }

        // SDL_RenderLine(renderer, player_data->position.x, player_data->position.y, ray_x, ray_y);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

        ray_angle = rotation_add_angle(ray_rotation, RCE_1D);

        draw_3d_walls(renderer, shortest_distance, rotation_get_angle(player_rotation) - ray_angle, ray_i, hit_horizontal_wall);
    }

    free(ray_rotation);
}

void draw_3d_walls(SDL_Renderer *renderer, float distance, float delta_angle, int ray_index, bool hit_horizontal_wall)
{
    distance = distance * cos(delta_angle);

    int line_height = (TILE_PIXEL_COUNT * SCREEN_HEIGHT) / distance;

    int line_offset = SCREEN_HEIGHT / 2 - (line_height >> 1);

    SDL_FRect line = {ray_index * LINE_WIDTH,
                      line_offset,
                      LINE_WIDTH,
                      line_height};

    Uint8 rgb = hit_horizontal_wall ? 200 : 255;

    SDL_SetRenderDrawColor(renderer, rgb, rgb, rgb, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &line);
    SDL_RenderRect(renderer, &line);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}