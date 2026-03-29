#include "raycaster.h"

ECS_SYSTEM_DECLARE(RaycasterUpdate);
ECS_SYSTEM_DECLARE(RaycasterDestroy);
ECS_TAG_DECLARE(Raycaster);

Uint32 buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

const int DRAW_START_MIN = 0;
const int DRAW_END_MAX = SCREEN_HEIGHT;

void RaycasterModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, RaycasterModule);

    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, SpriteModule);
    ECS_IMPORT(world, CameraModule);

    ECS_TAG_DEFINE(world, Raycaster);
    ECS_SYSTEM_DEFINE(world, RaycasterUpdate, EcsOnUpdate, Raycaster);
    ECS_SYSTEM_DEFINE(world, RaycasterDestroy, EcsOnDelete, Raycaster);

    SDL_Renderer *renderer = ecs_get(world, ecs_id(Renderer), Renderer)->ptr;

    pixels = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    if (pixels == NULL)
    {
        SDL_Log("Unabled to create pixels texture: %s\n", SDL_GetError());
        return;
    }

    // Generate some textures
    for (int i = 0; i < 8; i++)
    {
        textures[i] = (Uint32 *)malloc(TEXTURE_HEIGHT * TEXTURE_WIDTH * sizeof(Uint32));

        if (textures[i] == NULL)
        {
            SDL_Log("Unable to init textures.");
            return;
        }
    }

    for (int x = 0; x < TEXTURE_WIDTH; x++)
    {
        for (int y = 0; y < TEXTURE_HEIGHT; y++)
        {
            textures[0][x + (TEXTURE_WIDTH * y)] = 0xFFFF0000 * (x != y && x != TEXTURE_WIDTH - y); // flat red texture with black cross
        }
    }
}

void RaycasterUpdate(ecs_iter_t *it)
{
    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t player = ecs_lookup(it->world, PLAYER_ENTITY_NAME);
        SDL_Renderer *renderer = ecs_get(it->world, ecs_id(Renderer), Renderer)->ptr;
        SDL_Window *window = ecs_get(it->world, ecs_id(Window), Window)->ptr;

        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));

        draw(renderer, window, it->world, player);

        SDL_RenderTexture(renderer, pixels, NULL, NULL);

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

/*
Notes on this function:
Lodev's implementation of raycaster DDA algorithm
*/
void draw(SDL_Renderer *renderer, SDL_Window *window, ecs_world_t *world, ecs_entity_t player)
{
    const Position *pos = ecs_get(world, player, Position);
    const Direction *dir = ecs_get(world, player, Direction);
    const Plane *plane = ecs_get(world, player, Plane);

    int screen_width = SCREEN_WIDTH;
    int screen_height = SCREEN_HEIGHT;

    texture_pixels = NULL;
    texture_pitch = screen_width * sizeof(Uint32);

    if (!SDL_LockTexture(pixels, NULL, &texture_pixels, &texture_pitch))
        return;

    // This loop draws the whole frame
    for (int x = 0; x < screen_width; x++)
    {
        struct DdaData dda_data = {0};

        dda(pos, dir, plane, x, screen_width, screen_height, &dda_data);

        int line_height = (int)(screen_height / dda_data.perp_wall_dist);
        int draw_start = (int)fmax(-line_height / 2 + screen_height / 2, DRAW_START_MIN);
        int draw_end = (int)fmin(line_height / 2 + screen_height / 2, DRAW_END_MAX);

        // --------- Drawing starts ---------
        // int tex_num = world_map[map_x][map_y] - 1;

        // calculate value of wallX
        double wall_x = dda_data.side_orientation == HORIZONTAL
                            ? pos->y + dda_data.perp_wall_dist * dda_data.ray_direction.y
                            : pos->x + dda_data.perp_wall_dist * dda_data.ray_direction.x;

        wall_x -= floor(wall_x);

        // x coordinate on the texture
        int tex_x = (int)(wall_x * (double)TEXTURE_WIDTH);

        if (dda_data.side_orientation == HORIZONTAL && dda_data.ray_direction.x > 0)
            tex_x = TEXTURE_WIDTH - tex_x - 1;

        if (dda_data.side_orientation == VERTICAL && dda_data.ray_direction.y < 0)
            tex_x = TEXTURE_WIDTH - tex_x - 1;

        double step = 1.0 * TEXTURE_HEIGHT / line_height;

        // Starting texture coordinate
        double tex_pos = (draw_start - screen_height / 2 + line_height / 2) * step;

        for (int y = draw_start; y < draw_end; y++)
        {
            // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
            int tex_y = (int)tex_pos & (TEXTURE_HEIGHT - 1);
            tex_pos += step;

            Uint32 color = textures[0][TEXTURE_HEIGHT * tex_y + tex_x];

            // make color darker for y-sides
            if (dda_data.side_orientation == VERTICAL)
                color = (color >> 1) & 0xFF7F7F7F;

            buffer[y][x] = color;
        }
    }

    memcpy(texture_pixels, buffer, sizeof(buffer));

    SDL_UnlockTexture(pixels);
}

void dda(const Position *position, const Direction *direction, const Plane *plane, int screen_x, int screen_width, int screen_height, struct DdaData *output_dda_data)
{
    double camera_x = 2 * screen_x / (double)screen_width - 1;

    Vector2 ray_direction = {direction->x + plane->x * camera_x, direction->y + plane->y * camera_x};
    Vector2I ray_origin = {(int)floorf(position->x), (int)floorf(position->y)};

    float dist_to_x;
    float dist_to_y;

    double dist_between_cols = (ray_direction.x == 0) ? 1e30 : fabs(1 / ray_direction.x);
    double dist_between_rows = (ray_direction.y == 0) ? 1e30 : fabs(1 / ray_direction.y);

    int step_x;
    int step_y;

    bool has_hit_side = false;
    enum Orientation side_orientation;

    if (ray_direction.x < 0)
    {
        step_x = -1;
        dist_to_x = (position->x - ray_origin.x) * dist_between_cols;
    }
    else
    {
        step_x = 1;
        dist_to_x = (ray_origin.x + 1 - position->x) * dist_between_cols;
    }

    if (ray_direction.y < 0)
    {
        step_y = -1;
        dist_to_y = (position->y - ray_origin.y) * dist_between_rows;
    }
    else
    {
        step_y = 1;
        dist_to_y = (ray_origin.y + 1 - position->y) * dist_between_rows;
    }

    while (!has_hit_side)
    {
        if (dist_to_x < dist_to_y)
        {
            dist_to_x += dist_between_cols;
            ray_origin.x += step_x;
            side_orientation = HORIZONTAL;
        }
        else
        {
            dist_to_y += dist_between_rows;
            ray_origin.y += step_y;
            side_orientation = VERTICAL;
        }

        if (world_map[ray_origin.x][ray_origin.y] > 0)
            has_hit_side = true;
    }

    output_dda_data->side_orientation = side_orientation;
    output_dda_data->ray_direction = ray_direction;
    output_dda_data->dist_to_x = dist_to_x;
    output_dda_data->dist_to_y = dist_to_y;
    output_dda_data->perp_wall_dist = side_orientation == HORIZONTAL
                                          ? dist_to_x - dist_between_cols
                                          : dist_to_y - dist_between_rows;
}