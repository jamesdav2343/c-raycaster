#include "raycaster.h"

ECS_SYSTEM_DECLARE(RaycasterUpdate);
ECS_SYSTEM_DECLARE(RaycasterDestroy);
ECS_SYSTEM_DECLARE(RaycasterDraw);
ECS_TAG_DECLARE(Raycaster);
ECS_COMPONENT_DECLARE(BufferData);

static SDL_Texture *pixels_texture;
static Uint32 *textures[8];

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
    ECS_SYSTEM_DEFINE(world, RaycasterDraw, EcsOnStore, Raycaster);
    ECS_COMPONENT_DEFINE(world, BufferData);

    SDL_Renderer *renderer = ecs_singleton_get(world, Renderer)->value;

    pixels_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    if (pixels_texture == NULL)
    {
        SDL_Log("Unabled to create pixels texture: %s\n", SDL_GetError());
        return;
    }

    BufferData buffer_data = {0};
    buffer_data.buffer = (Uint32 *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
    buffer_data.width = SCREEN_WIDTH;
    buffer_data.height = SCREEN_HEIGHT;
    buffer_data.size = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32);

    ecs_add_id(world, ecs_id(BufferData), EcsSingleton);
    ecs_set_ptr(world, ecs_id(BufferData), BufferData, &buffer_data);

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

    SDL_Surface *wall = IMG_Load("assets/Wall_1.png");
    SDL_Surface *wall_1 = SDL_ConvertSurface(wall, SDL_PIXELFORMAT_ARGB8888);
    SDL_DestroySurface(wall);

    SDL_Surface *floor = IMG_Load("assets/Floor_1.png");
    SDL_Surface *floor_1 = SDL_ConvertSurface(floor, SDL_PIXELFORMAT_ARGB8888);
    SDL_DestroySurface(floor);

    SDL_Surface *ceiling = IMG_Load("assets/Ceiling_1.png");
    SDL_Surface *ceiling_1 = SDL_ConvertSurface(ceiling, SDL_PIXELFORMAT_ARGB8888);
    SDL_DestroySurface(ceiling);

    textures[0] = (Uint32 *)wall_1->pixels;
    textures[1] = (Uint32 *)floor_1->pixels;
    textures[2] = (Uint32 *)ceiling_1->pixels;
}

void RaycasterUpdate(ecs_iter_t *it)
{
    ecs_entity_t player = ecs_lookup(it->world, PLAYER_ENTITY_NAME);
    const Position *position = ecs_get(it->world, player, Position);
    const Direction *direction = ecs_get(it->world, player, Direction);
    const Plane *plane = ecs_get(it->world, player, Plane);

    BufferData *buffer_data = ecs_singleton_get_mut(it->world, BufferData);

    clear_buffer(buffer_data);
    write_to_buffer(position, direction, plane, buffer_data);
    blit_buffer_to_texture(pixels_texture, buffer_data);
}

void RaycasterDraw(ecs_iter_t *it)
{
    SDL_Renderer *renderer = ecs_singleton_get(it->world, Renderer)->value;

    SDL_RenderTexture(renderer, pixels_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

void clear_buffer(BufferData *buffer_data)
{
    memset(buffer_data->buffer, 0, buffer_data->size);
}

void write_to_buffer(const Position *position, const Direction *direction, const Plane *plane, BufferData *dest_buffer_data)
{
    int screen_width = dest_buffer_data->width;
    int screen_height = dest_buffer_data->height;

    write_floor_and_celing(position, direction, plane, dest_buffer_data, screen_width, screen_height);

    // Wall casting for the current frame
    for (int x = 0; x < screen_width; x++)
    {
        struct DdaData dda_data = {0};

        dda(position, direction, plane, x, screen_width, screen_height, &dda_data);
        write_vertical_wall_strip(&dda_data, position, x, dest_buffer_data);
    }
}

void blit_buffer_to_texture(SDL_Texture *dest_pixels_texture, BufferData *src_buffer_data)
{
    void *pixels_buffer = NULL;
    int texture_pitch = src_buffer_data->width * sizeof(Uint32);

    if (!SDL_LockTexture(dest_pixels_texture, NULL, &pixels_buffer, &texture_pitch))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error locking pixels texture.");
        return;
    }

    memcpy(pixels_buffer, src_buffer_data->buffer, src_buffer_data->size);

    SDL_UnlockTexture(pixels_texture);
}

void write_vertical_wall_strip(struct DdaData *dda_data, const Position *position, int current_x, BufferData *dest_buffer_data)
{
    int buffer_height = dest_buffer_data->height;
    int buffer_width = dest_buffer_data->width;

    int line_height = (int)(buffer_height / dda_data->perp_wall_dist);
    int draw_start = (int)fmax(-line_height / 2 + buffer_height / 2, DRAW_START_MIN);
    int draw_end = (int)fmin(line_height / 2 + buffer_height / 2, DRAW_END_MAX);

    // calculate value of wallX
    double wall_x = dda_data->side_orientation == HORIZONTAL
                        ? position->y + dda_data->perp_wall_dist * dda_data->ray_direction.y
                        : position->x + dda_data->perp_wall_dist * dda_data->ray_direction.x;

    wall_x -= floor(wall_x);

    // x coordinate on the texture
    int tex_x = (int)(wall_x * (double)TEXTURE_WIDTH);

    if (dda_data->side_orientation == HORIZONTAL && dda_data->ray_direction.x > 0)
        tex_x = TEXTURE_WIDTH - tex_x - 1;

    if (dda_data->side_orientation == VERTICAL && dda_data->ray_direction.y < 0)
        tex_x = TEXTURE_WIDTH - tex_x - 1;

    double step = 1.0 * TEXTURE_HEIGHT / line_height;

    // Starting texture coordinate
    double texture_coord = (draw_start - buffer_height / 2 + line_height / 2) * step;

    float darkness_level = fminf(MAX_SHADOW, fmaxf(MIN_SHADOW, dda_data->perp_wall_dist / 5));

    for (int y = draw_start; y < draw_end; y++)
    {
        // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
        int tex_y = (int)texture_coord & (TEXTURE_HEIGHT - 1);
        texture_coord += step;

        Uint32 color = textures[0][TEXTURE_HEIGHT * tex_y + tex_x];

        color = color_lerp(color, BLACK, darkness_level) | 0xFF000000;

        dest_buffer_data->buffer[current_x + (y * buffer_width)] = color;
    }
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

void write_floor_and_celing(const Position *position, const Direction *direction, const Plane *plane, BufferData *dest_buffer_data, int screen_width, int screen_height)
{
    float ray_dir_x_0 = direction->x - plane->x;
    float ray_dir_y_0 = direction->y - plane->y;
    float ray_dir_x_1 = direction->x + plane->x;
    float ray_dir_y_1 = direction->y + plane->y;

    float pos_z = 0.5 * screen_height;

    int starting_y = screen_height / 2 + 1;

    // Floor casting
    for (int y = starting_y; y < screen_height; ++y)
    {
        int p = y - screen_height / 2;

        float row_distance = pos_z / p;

        float floor_step_x = row_distance * (ray_dir_x_1 - ray_dir_x_0) / screen_width;
        float floor_step_y = row_distance * (ray_dir_y_1 - ray_dir_y_0) / screen_width;

        float floor_x = position->x + row_distance * ray_dir_x_0;
        float floor_y = position->y + row_distance * ray_dir_y_0;

        float darkness_level = 1.0f - fminf(MAX_SHADOW, fmaxf(MIN_SHADOW, (float)y / SCREEN_HEIGHT)) + 0.5f;

        for (int x = 0; x < screen_width; ++x)
        {
            int cell_x = (int)(floor_x);
            int cell_y = (int)(floor_y);

            int texture_x = (int)(TEXTURE_WIDTH * (floor_x - cell_x)) & (TEXTURE_WIDTH - 1);
            int texture_y = (int)(TEXTURE_HEIGHT * (floor_y - cell_y)) & (TEXTURE_HEIGHT - 1);

            floor_x += floor_step_x;
            floor_y += floor_step_y;

            Uint32 colour;

            // Floor colour
            colour = textures[1][TEXTURE_WIDTH * texture_y + texture_x];
            colour = color_lerp(colour, BLACK, darkness_level) | ALPHA_OPAQUE_HEX;

            dest_buffer_data->buffer[x + (y * screen_width)] = colour;

            // Ceiling colour
            colour = textures[2][TEXTURE_WIDTH * texture_y + texture_x];
            colour = color_lerp(colour, BLACK, darkness_level) | ALPHA_OPAQUE_HEX;
            dest_buffer_data->buffer[x + ((screen_height - y - 1) * screen_width)] = colour;
        }
    }
}