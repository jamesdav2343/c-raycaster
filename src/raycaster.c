#include "raycaster.h"

void ver_line(SDL_Renderer *renderer, int x, int y1, int y2, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderLine(renderer, x, y1, x, y2);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

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
Lodev's implementation of raycaster DDA algorithm
*/
void draw_rays(SDL_Renderer *renderer, ecs_world_t *world, ecs_entity_t player)
{
    const bool *key_states = SDL_GetKeyboardState(NULL);

    Position *player_position = ecs_get_mut(world, player, Position);
    Direction *player_direction = ecs_get_mut(world, player, Direction);
    CameraPlane *camera_plane = ecs_get_mut(world, player, CameraPlane);

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

    double move_speed = 0.1; // the constant value is in squares/second
    double rot_speed = 0.1;  // the constant value is in radians/second
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

// void draw_3d_walls(SDL_Renderer *renderer, float distance, float delta_angle, int ray_index, bool hit_horizontal_wall)
// {
//     distance = distance * cos(delta_angle);

//     int line_height = (TILE_PIXEL_COUNT * SCREEN_HEIGHT) / distance;

//     int line_offset = SCREEN_HEIGHT / 2 - (line_height / 2);

//     SDL_FRect line = {ray_index * LINE_WIDTH,
//                       line_offset,
//                       LINE_WIDTH,
//                       line_height};

//     Uint8 rgb = hit_horizontal_wall ? 200 : 255;

//     SDL_SetRenderDrawColor(renderer, rgb, rgb, rgb, SDL_ALPHA_OPAQUE);
//     SDL_RenderFillRect(renderer, &line);
//     SDL_RenderRect(renderer, &line);
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
// }