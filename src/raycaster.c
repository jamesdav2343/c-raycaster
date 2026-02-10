#include "raycaster.h"

void draw_rays(SDL_Renderer *renderer, PlayerData *player_data)
{
    int map_x, map_y, map_position, depth_of_field;
    float ray_x, ray_y, ray_angle, x_offset, y_offset;

    ray_angle = player_data->angle;

    for (int ray = 0; ray < 1; ray++)
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
        if (ray_angle < M_PI)
        {
            ray_y = (((int)player_data->position.y >> 6) << 6) + TILE_PIXEL_COUNT;
            ray_x = (player_data->position.y - ray_y) * aTan + player_data->position.x;
            y_offset = TILE_PIXEL_COUNT;
            x_offset = -y_offset * aTan;
        }

        // Looking straight left or right
        if (ray_angle == 0 || ray_angle == M_PI - EPSILON)
        {
            printf("in floating point comparison conditional\n");
            ray_x = player_data->position.x;
            ray_y = player_data->position.y;
            depth_of_field = DOF_MAX;
        }

        while (depth_of_field < DOF_MAX)
        {
            map_x = fmax((int)ray_x >> 6, MAP_COORD_MIN);
            map_y = fmax((int)ray_y >> 6, MAP_COORD_MIN);
            map_position = map_y * COLS + map_x;

            printf("(%d, %d)\n", map_x, map_y);

            if (map_position < COLS * ROWS && map[map_y][map_x] >= 1)
            {
                depth_of_field = DOF_MAX;
            }
            else
            {
                ray_x += x_offset;
                ray_y += y_offset;
                depth_of_field += 1;
            }

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderLine(renderer, player_data->position.x, player_data->position.y, ray_x, ray_y);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        }
    }
}