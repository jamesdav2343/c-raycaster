#include "raycaster.h"

void draw_rays(SDL_Renderer *renderer, PlayerData *player_data)
{
    int map_x, map_y, map_position, depth_of_field;
    float ray_x, ray_y, ray_angle, x_offset, y_offset;

    ray_angle = player_data->angle;

    for (int r = 0; r < 1; r++)
    {
        // Check horizontal lines
        depth_of_field = 0;
        float aTan = -1 / tan(ray_angle);

        // Looking up
        if (ray_angle > M_PI)
        {
            ray_y = (((int)player_data->position.y >> 6) << 6) - 0.0001;
            ray_x = (player_data->position.y - ray_y) * aTan + player_data->position.x;
            y_offset = -64;
            x_offset = -y_offset * aTan;
        }

        // Looking down
        if (ray_angle < M_PI)
        {
            ray_y = (((int)player_data->position.y >> 6) << 6) + 64;
            ray_x = (player_data->position.y - ray_y) * aTan + player_data->position.x;
            y_offset = 64;
            x_offset = -y_offset * aTan;
        }

        // Looking straight left or right
        if (ray_angle == 0 || ray_angle == M_PI)
        {
            ray_x = player_data->position.x;
            ray_y = player_data->position.y;
            depth_of_field = 8;
        }

        while (depth_of_field < 8)
        {
            map_x = (int)ray_x >> 6;
            map_y = (int)(ray_y) >> 6;
            map_position = map_y * cols + map_x;

            if (map_position < cols * rows && *map[map_position] == 1)
            {
                depth_of_field = 8;
            }
            else
            {
                ray_x += x_offset;
                ray_y += y_offset;
                depth_of_field += 1;
            }

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderLine(renderer, player_data->position.x, player_data->position.y, ray_x, ray_y);
        }
    }
}