#include "systems/raycaster.h"
#include "components/game_manager.h"
#include "types.h"
#include <SDL3_image/SDL_image.h>

ECS_SYSTEM_DECLARE(RaycasterUpdate);
ECS_SYSTEM_DECLARE(RaycasterDraw);

/**
 * Runtime globals.
 */
static SDL_Texture* pixels_texture;
static ht* texture_map;
static int DRAW_START_MIN;
static int DRAW_END_MAX;
static double* z_buffer = NULL;

#define SPRITE_COUNT 19
Sprite sprite[SPRITE_COUNT] = { { 12, 14, 3 }, // pillar
    { 12, 15, 4 } }; // light
// arrays used to sort the sprites
int sprite_order[SPRITE_COUNT];
double sprite_distance[SPRITE_COUNT];

/**
 * Casts a ray.
 * Great explanation of the algorithm here:
 * https://aaaa.sh/creatures/dda-algorithm-interactive/
 */
static void dda(const Position* position, const Direction* direction, const Plane* plane, int screen_x,
    int screen_width, int screen_height, Ray* ray_out)
{
    double camera_x = 2 * screen_x / (double)screen_width - 1;

    Vector2 ray_direction = { direction->x + plane->x * camera_x, direction->y + plane->y * camera_x };
    Vector2I ray_origin = { (int)floorf(position->x), (int)floorf(position->y) };

    float dist_to_x;
    float dist_to_y;

    double dist_between_cols = (ray_direction.x == 0) ? 1e30 : fabs(1 / ray_direction.x);
    double dist_between_rows = (ray_direction.y == 0) ? 1e30 : fabs(1 / ray_direction.y);

    int step_x;
    int step_y;

    bool has_hit_side = false;
    enum Orientation side_orientation;

    if (ray_direction.x < 0) {
        step_x = -1;
        dist_to_x = (position->x - ray_origin.x) * dist_between_cols;
    } else {
        step_x = 1;
        dist_to_x = (ray_origin.x + 1 - position->x) * dist_between_cols;
    }

    if (ray_direction.y < 0) {
        step_y = -1;
        dist_to_y = (position->y - ray_origin.y) * dist_between_rows;
    } else {
        step_y = 1;
        dist_to_y = (ray_origin.y + 1 - position->y) * dist_between_rows;
    }

    while (!has_hit_side) {
        if (dist_to_x < dist_to_y) {
            dist_to_x += dist_between_cols;
            ray_origin.x += step_x;
            side_orientation = VERTICAL;
        } else {
            dist_to_y += dist_between_rows;
            ray_origin.y += step_y;
            side_orientation = HORIZONTAL;
        }

        if (world_map[ray_origin.x + (ray_origin.y * COLS)] > 0)
            has_hit_side = true;
    }

    ray_out->direction = ray_direction;
    ray_out->dist_to_x = dist_to_x;
    ray_out->dist_to_y = dist_to_y;
    ray_out->perp_wall_dist
        = side_orientation == VERTICAL ? dist_to_x - dist_between_cols : dist_to_y - dist_between_rows;
    ray_out->wall.side_orientation = side_orientation;
    ray_out->wall.wall_position = ray_origin;
}

/**
 * Resets the pixel buffer. Sets all values to 0.
 */
static void clear_buffer(PixelBuffer* pixel_buffer) { memset(pixel_buffer->pixels, 0, pixel_buffer->size); }

static void blit_buffer_to_texture(SDL_Texture* dest_pixels_texture, PixelBuffer* src_pixel_buffer)
{
    void* pixels_buffer = NULL;
    int texture_pitch = src_pixel_buffer->width * sizeof(Uint32);

    if (!SDL_LockTexture(dest_pixels_texture, NULL, &pixels_buffer, &texture_pitch)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error locking pixels texture.");
        return;
    }

    memcpy(pixels_buffer, src_pixel_buffer->pixels, src_pixel_buffer->size);

    SDL_UnlockTexture(pixels_texture);
}

/**
 *  Writes a vertical wall strip to the pixel buffer.
 */
static void write_vertical_wall_strip(Ray* ray, const Position* position, int current_x, PixelBuffer* dest_buffer_data)
{
    ht* textures = (ht*)ht_get(texture_map, "walls");
    Uint8 wall_id = world_map[ray->wall.wall_position.x + (ray->wall.wall_position.y * COLS)];

    int buffer_len = 1024;

    char buffer[buffer_len];
    snprintf(buffer, buffer_len, "%d", wall_id);
    SDL_Surface* wall = (SDL_Surface*)ht_get(textures, buffer);

    Uint32* tex_pixels = wall != NULL ? (Uint32*)wall->pixels : NULL;

    int tex_width = wall != NULL ? wall->w : TEXTURE_WIDTH_FALLBACK;
    int tex_height = wall != NULL ? wall->h : TEXTURE_HEIGHT_FALLBACK;

    int buffer_height = dest_buffer_data->height;
    int buffer_width = dest_buffer_data->width;

    int line_height = (int)(buffer_height / ray->perp_wall_dist);
    int draw_start = (int)fmax(-line_height / 2 + buffer_height / 2, DRAW_START_MIN);
    int draw_end = (int)fmin(line_height / 2 + buffer_height / 2, DRAW_END_MAX);

    double wall_x = ray->wall.side_orientation == VERTICAL ? position->y + ray->perp_wall_dist * ray->direction.y
                                                           : position->x + ray->perp_wall_dist * ray->direction.x;

    wall_x -= floor(wall_x);

    int tex_x = (int)(wall_x * (double)tex_width);

    if (ray->wall.side_orientation == HORIZONTAL && ray->direction.x > 0)
        tex_x = tex_width - tex_x - 1;

    if (ray->wall.side_orientation == VERTICAL && ray->direction.y < 0)
        tex_x = tex_width - tex_x - 1;

    double tex_step = 1.0 * tex_height / line_height;

    double tex_coord = (draw_start - buffer_height / 2 + line_height / 2) * tex_step;

    float base_lighting_level = 1.0f
        - get_wall_light_intensity(
            ray->wall.wall_position.x, ray->wall.wall_position.y, ray->direction, ray->wall.side_orientation);

    for (int y = draw_start; y < draw_end; y++) {
        // Masked with tex_height - 1 in case of overflow
        int tex_y = (int)tex_coord & (tex_height - 1);
        tex_coord += tex_step;

        Uint32 color = tex_pixels != NULL ? tex_pixels[tex_x + (tex_y * tex_width)] : WHITE;

        color = interpolate(color, BLACK, base_lighting_level) | ALPHA_OPAQUE_HEX;

        dest_buffer_data->pixels[current_x + (y * buffer_width)] = color;
    }
}

/**
 * Writes the horizontal wall and ceiling strips to the pixel buffer.
 */
static void write_floor_and_celing(const Position* position, const Direction* direction, const Plane* plane,
    PixelBuffer* dest_buffer_data, int screen_width, int screen_height)
{
    ht* textures = (ht*)ht_get(texture_map, "floors");
    SDL_Surface* floor = (SDL_Surface*)ht_get(textures, "1");
    Uint32* tex_pixels = floor != NULL ? floor->pixels : NULL;

    int tex_width = floor != NULL ? floor->w : TEXTURE_WIDTH_FALLBACK;
    int tex_height = floor != NULL ? floor->h : TEXTURE_HEIGHT_FALLBACK;

    float ray_dir_x_0 = direction->x - plane->x;
    float ray_dir_y_0 = direction->y - plane->y;
    float ray_dir_x_1 = direction->x + plane->x;
    float ray_dir_y_1 = direction->y + plane->y;

    float pos_z = 0.5 * screen_height;

    int starting_y = screen_height / 2 + 1;

    for (int y = starting_y; y < screen_height; ++y) {
        int p = y - screen_height / 2;

        float row_distance = pos_z / p;

        float floor_step_x = row_distance * (ray_dir_x_1 - ray_dir_x_0) / screen_width;
        float floor_step_y = row_distance * (ray_dir_y_1 - ray_dir_y_0) / screen_width;

        float floor_x = position->x + row_distance * ray_dir_x_0;
        float floor_y = position->y + row_distance * ray_dir_y_0;

        for (int x = 0; x < screen_width; ++x) {
            int cell_x = (int)floor_x;
            int cell_y = (int)floor_y;

            float base_lighting_level = 1.0f - light_map[cell_x + (cell_y * COLS)];

            int texture_x = (int)(tex_width * (floor_x - cell_x)) & (tex_width - 1);
            int texture_y = (int)(tex_height * (floor_y - cell_y)) & (tex_height - 1);

            floor_x += floor_step_x;
            floor_y += floor_step_y;

            Uint32 colour;

            // Floor colour
            colour = tex_pixels[texture_x + (tex_width * texture_y)];

            // If its the light source, draw in red
            if (light_map[cell_x + (cell_y * COLS)] >= 1.0f) {
                colour = RED;
            }

            colour = interpolate(colour, BLACK, base_lighting_level) | ALPHA_OPAQUE_HEX;

            dest_buffer_data->pixels[x + (y * screen_width)] = colour;

            // Ceiling colour
            colour = tex_pixels[texture_x + (tex_width * texture_y)];

            colour = interpolate(colour, BLACK, base_lighting_level) | ALPHA_OPAQUE_HEX;
            dest_buffer_data->pixels[x + ((screen_height - y - 1) * screen_width)] = colour;
        }
    }
}

/**
 * Writes the sprites to the pixel buffer.
 */
static void write_sprites(const Position* position, const Direction* direction, const Plane* plane,
    PixelBuffer* dest_pixel_buffer, int screen_width, int screen_height)
{
    // Sorts the sprites from far to close
    for (int i = 0; i < SPRITE_COUNT; i++) {
        sprite_order[i] = i;
        sprite_distance[i] = ((position->x - sprite[i].x) * (position->x - sprite[i].x)
            + (position->y - sprite[i].y) * (position->y - sprite[i].y));
    }

    sort_sprites(sprite_order, sprite_distance, SPRITE_COUNT);

    for (int i = 0; i < SPRITE_COUNT; i++) {
        double sprite_x = sprite[sprite_order[i]].x - position->x;
        double sprite_y = sprite[sprite_order[i]].y - position->y;

        double inv_det
            = 1.0 / (plane->x * direction->y - direction->x * plane->y); // required for correct matrix multiplication

        double transform_x = inv_det * (direction->y * sprite_x - direction->x * sprite_y);
        double transform_y = inv_det * (-plane->y * sprite_x + plane->x * sprite_y);

        int sprite_screen_x = (int)((screen_width / 2) * (1 + transform_x / transform_y));

        int sprite_height = abs((int)(screen_height / (transform_y)));

        int draw_start_y = -sprite_height / 2 + screen_height / 2;
        if (draw_start_y < 0)
            draw_start_y = 0;

        int draw_end_y = sprite_height / 2 + screen_height / 2;
        if (draw_end_y >= screen_height)
            draw_end_y = screen_height - 1;

        int sprite_width = abs((int)(screen_height / (transform_y)));
        int draw_start_x = -sprite_width / 2 + sprite_screen_x;
        if (draw_start_x < 0)
            draw_start_x = 0;

        int draw_end_x = sprite_width / 2 + sprite_screen_x;
        if (draw_end_x >= screen_width)
            draw_end_x = screen_width - 1;

        // texture stuff
        ht* sprites = (ht*)ht_get(texture_map, "sprites");

        if (sprites == NULL) {
            printf("sprites is null\n");
            return;
        }

        SDL_Surface* surface = (SDL_Surface*)ht_get(sprites, "1");

        if (surface == NULL) {
            printf("could not get surface\n");
            return;
        }

        Uint32* pixels = (Uint32*)((SDL_Surface*)ht_get(sprites, "1"))->pixels;

        for (int stripe = draw_start_x; stripe < draw_end_x; stripe++) {
            int tex_x
                = (int)(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) * SPRITE_TEXTURE_WIDTH / sprite_width)
                / 256;

            if (transform_y > 0 && stripe > 0 && stripe < screen_width && transform_y < z_buffer[stripe]) {

                for (int y = draw_start_y; y < draw_end_y; y++) // for every pixel of the current stripe
                {
                    // 256 and 128 factors to avoid floats
                    int d = (y) * 256 - screen_height * 128 + sprite_height * 128;

                    int tex_y = ((d * SPRITE_TEXTURE_HEIGHT) / sprite_height) / 256;

                    // get current color from the texture
                    // Uint32 color = textures[sprite[sprite_order[i]].texture][SPRITE_TEXTURE_WIDTH * tex_y + tex_x];
                    Uint32 color = pixels[SPRITE_TEXTURE_WIDTH * tex_y + tex_x];
                    // Uint32 color = WHITE;

                    if ((color & 0x00FFFFFF) != 0) {
                        // paint pixel if it isn't black, black is the invisible color
                        dest_pixel_buffer->pixels[stripe + (y * dest_pixel_buffer->width)] = color;
                    }
                }
            }
        }
    }
}

/**
 * The main function called every frame.
 */
static void write_to_buffer(
    const Position* position, const Direction* direction, const Plane* plane, PixelBuffer* dest_pixel_buffer)
{
    int screen_width = dest_pixel_buffer->width;
    int screen_height = dest_pixel_buffer->height;

    write_floor_and_celing(position, direction, plane, dest_pixel_buffer, screen_width, screen_height);

    for (int x = 0; x < screen_width; x++) {
        Ray ray = { 0 };

        dda(position, direction, plane, x, screen_width, screen_height, &ray);
        write_vertical_wall_strip(&ray, position, x, dest_pixel_buffer);

        z_buffer[x] = ray.perp_wall_dist;
    }

    write_sprites(position, direction, plane, dest_pixel_buffer, screen_width, screen_height);
}

/**
 * Cleanup function. Frees resources.
 */
static void raycaster_cleanup(ecs_world_t* world, void* ctx)
{
    if (z_buffer != NULL) {
        free(z_buffer);
        z_buffer = NULL;
    }

    const PixelBuffer* pixel_buffer = ecs_get(world, ecs_id(PixelBuffer), PixelBuffer);

    if (pixel_buffer->pixels != NULL) {
        free(pixel_buffer->pixels);
    }
}

void RaycasterSystemsImport(ecs_world_t* world)
{
    ECS_IMPORT(world, RaycasterComponents);
    ECS_IMPORT(world, TransformComponents);
    ECS_IMPORT(world, GameManagerComponents);

    ECS_IMPORT(world, SpriteModule);
    ECS_IMPORT(world, CameraModule);

    ECS_SYSTEM_DEFINE(world, RaycasterUpdate, EcsOnUpdate, Raycaster);
    ECS_SYSTEM_DEFINE(world, RaycasterDraw, EcsOnStore, Raycaster);

    const VideoConfig* video_config = ecs_singleton_get(world, VideoConfig);

    z_buffer = (double*)calloc(video_config->screen_size.x, sizeof(double));

    SDL_Renderer* renderer = ecs_singleton_get(world, Renderer)->value;

    int screen_width = video_config->screen_size.x;
    int screen_height = video_config->screen_size.y;

    DRAW_START_MIN = 0;
    DRAW_END_MAX = screen_height;

    pixels_texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);

    if (pixels_texture == NULL) {
        SDL_Log("Unabled to create pixels texture: %s\n", SDL_GetError());
        return;
    }

    PixelBuffer buffer_data = { 0 };
    buffer_data.pixels = (Uint32*)malloc(screen_width * screen_height * sizeof(Uint32));
    buffer_data.width = screen_width;
    buffer_data.height = screen_height;
    buffer_data.size = screen_width * screen_height * sizeof(Uint32);

    ecs_add_id(world, ecs_id(PixelBuffer), EcsSingleton);
    ecs_set_ptr(world, ecs_id(PixelBuffer), PixelBuffer, &buffer_data);

    ecs_atfini(world, raycaster_cleanup, NULL);

    // Generate some textures
    ECS_MODULE(world, RaycasterSystems);

    ht* textures_config = ecs_singleton_get(world, TexturesConfig)->config;

    texture_map = create_textures_from_config(textures_config);
}

void RaycasterUpdate(ecs_iter_t* it)
{
    ecs_entity_t player = ecs_lookup(it->world, PLAYER_ENTITY_NAME);
    const Position* position = ecs_get(it->world, player, Position);
    const Direction* direction = ecs_get(it->world, player, Direction);
    const Plane* plane = ecs_get(it->world, player, Plane);

    PixelBuffer* buffer_data = ecs_singleton_get_mut(it->world, PixelBuffer);

    clear_buffer(buffer_data);
    write_to_buffer(position, direction, plane, buffer_data);
    blit_buffer_to_texture(pixels_texture, buffer_data);
}

void RaycasterDraw(ecs_iter_t* it)
{
    SDL_Renderer* renderer = ecs_singleton_get(it->world, Renderer)->value;
    SDL_Window* window = ecs_singleton_get(it->world, Window)->value;

    SDL_RenderTexture(renderer, pixels_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
    SDL_RenderClear(renderer);
}