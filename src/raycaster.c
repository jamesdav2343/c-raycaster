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
void draw_rays(SDL_Renderer *renderer, PlayerData *player_data)
{
    int map_x, map_y, depth_of_field;
    float ray_x, ray_y, ray_angle, x_offset, y_offset;

    ray_angle = player_data->angle - RCE_1D * 30;

    if (ray_angle < 0)
    {
        ray_angle += 2 * M_PI;
    }

    if (ray_angle > 2 * M_PI)
    {
        ray_angle -= 2 * M_PI;
    }

    for (int ray_i = 0; ray_i < 1; ray_i++)
    {
        // Check horizontal lines
        depth_of_field = 0;
        float aTan = -1 / tan(ray_angle);

        // Looking up
        if (ray_angle > M_PI)
        {
            ray_y = (((int)player_data->position.y >> 6) << 6) - EPSILON;
            ray_x = (player_data->position.y - ray_y) * aTan + player_data->position.x;
            y_offset = -TILE_PIXEL_COUNT;
            x_offset = -y_offset * aTan;
        }
        // Looking down
        else if (ray_angle < M_PI)
        {
            ray_y = (((int)player_data->position.y >> 6) << 6) + TILE_PIXEL_COUNT;
            ray_x = (player_data->position.y - ray_y) * aTan + player_data->position.x;
            y_offset = TILE_PIXEL_COUNT;
            x_offset = -y_offset * aTan;
        }
        // Looking straight left or right
        else
        {
            ray_x = player_data->position.x;
            ray_y = player_data->position.y;
            depth_of_field = DOF_MAX;
        }

        while (depth_of_field < DOF_MAX)
        {
            map_x = (int)ray_x >> 6;
            map_y = (int)ray_y >> 6;

            map_x = clamp_in_range(map_x, MAP_COORD_MIN, MAP_COORD_MAX);
            map_y = clamp_in_range(map_y, MAP_COORD_MIN, MAP_COORD_MAX);

            // printf("(%d, %d)\n", map_x, map_y);

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
            ray_x = (((int)player_data->position.x >> 6) << 6) - EPSILON;
            ray_y = (player_data->position.x - ray_x) * nTan + player_data->position.y;
            x_offset = -TILE_PIXEL_COUNT;
            y_offset = -x_offset * nTan;
        }
        // Looking right
        else if (ray_angle < M_PI_2 || ray_angle > RCE_3PI_2)
        {
            ray_x = (((int)player_data->position.x >> 6) << 6) + TILE_PIXEL_COUNT;
            ray_y = (player_data->position.x - ray_x) * nTan + player_data->position.y;
            x_offset = TILE_PIXEL_COUNT;
            y_offset = -x_offset * nTan;
        }
        // Looking straight up or down
        else
        {
            ray_x = player_data->position.x;
            ray_y = player_data->position.y;
            depth_of_field = DOF_MAX;
        }

        while (depth_of_field < DOF_MAX)
        {
            map_x = (int)ray_x >> 6;
            map_y = (int)ray_y >> 6;

            map_x = clamp_in_range(map_x, MAP_COORD_MIN, MAP_COORD_MAX);
            map_y = clamp_in_range(map_y, MAP_COORD_MIN, MAP_COORD_MAX);

            // printf("(%d, %d)\n", map_x, map_y);

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

        float h_distance = distance_between_two_points(horizontal_x, horizontal_y, player_data->position.x, player_data->position.y);
        float v_distance = distance_between_two_points(ray_x, ray_y, player_data->position.x, player_data->position.y);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

        if (h_distance <= v_distance)
            SDL_RenderLine(renderer, player_data->position.x, player_data->position.y, horizontal_x, horizontal_y);
        else
            SDL_RenderLine(renderer, player_data->position.x, player_data->position.y, ray_x, ray_y);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    }
}