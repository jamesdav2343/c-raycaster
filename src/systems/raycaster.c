#include "systems/raycaster.h"
#include "components/game_manager.h"
#include "types.h"
#include <SDL3_image/SDL_image.h>

#define SPRITE_COUNT 19
static int DRAW_START_MIN;
static int DRAW_END_MAX;
static int sprite_order[SPRITE_COUNT];
static double sprite_distance[SPRITE_COUNT];

/**
 * Casts a ray.
 * Great explanation of the algorithm here:
 * https://aaaa.sh/creatures/dda-algorithm-interactive/
 */
static void dda(const Position* position, const Direction* direction, const CameraPlane* plane, int screen_x,
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
    enum Orientation orientation;

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
            orientation = ray_direction.x < 0.0f ? WEST : EAST;
        } else {
            dist_to_y += dist_between_rows;
            ray_origin.y += step_y;
            orientation = ray_direction.y < 0.0f ? SOUTH : NORTH;
        }

        if (world_map[ray_origin.x + (ray_origin.y * COLS)] > 0)
            has_hit_side = true;
    }

    ray_out->direction = ray_direction;
    ray_out->dist_to_x = dist_to_x;
    ray_out->dist_to_y = dist_to_y;
    ray_out->perp_wall_dist
        = orientation == WEST || orientation == EAST ? dist_to_x - dist_between_cols : dist_to_y - dist_between_rows;
    ray_out->wall.orientation = orientation;
    ray_out->wall.position = ray_origin;
}

/**
 *  Writes a vertical wall strip to the pixel buffer.
 */
static void write_vertical_wall_strip(
    Ray* ray, const Position* position, int current_x, PixelBuffer* dest_buffer_data, ht* texture_map)
{
    ht* textures = (ht*)ht_get(texture_map, "walls");
    Uint8 wall_id = world_map[ray->wall.position.x + (ray->wall.position.y * COLS)];

    char buffer[BUFFER_MAX];
    SDL_itoa(wall_id, buffer, 10);
    SDL_Surface* wall = (SDL_Surface*)ht_get(textures, buffer);

    Uint32* tex_pixels = wall != NULL ? (Uint32*)wall->pixels : NULL;

    int tex_width = wall != NULL ? wall->w : TEXTURE_WIDTH_FALLBACK;
    int tex_height = wall != NULL ? wall->h : TEXTURE_HEIGHT_FALLBACK;

    int buffer_height = dest_buffer_data->height;
    int buffer_width = dest_buffer_data->width;

    int line_height = (int)(buffer_height / ray->perp_wall_dist);
    int draw_start = (int)fmax(-line_height / 2 + buffer_height / 2, DRAW_START_MIN);
    int draw_end = (int)fmin(line_height / 2 + buffer_height / 2, DRAW_END_MAX);

    double wall_x = ray->wall.orientation == NORTH ? position->y + ray->perp_wall_dist * ray->direction.y
                                                   : position->x + ray->perp_wall_dist * ray->direction.x;

    wall_x -= floor(wall_x);

    int tex_x = (int)(wall_x * (double)tex_width);

    if (ray->wall.orientation == SOUTH && ray->direction.x > 0)
        tex_x = tex_width - tex_x - 1;

    if (ray->wall.orientation == NORTH && ray->direction.y < 0)
        tex_x = tex_width - tex_x - 1;

    double tex_step = 1.0 * tex_height / line_height;

    double tex_coord = (draw_start - buffer_height / 2 + line_height / 2) * tex_step;

    float base_lighting_level = 1.0f
        - get_wall_light_intensity(ray->wall.position.x, ray->wall.position.y, ray->direction, ray->wall.orientation);

    int pos = ray->wall.position.x + (ray->wall.position.y * COLS);

    for (int y = draw_start; y < draw_end; y++) {
        // Masked with tex_height - 1 in case of overflow
        int tex_y = (int)tex_coord & (tex_height - 1);
        tex_coord += tex_step;

        Uint32 color = tex_pixels != NULL ? tex_pixels[tex_x + (tex_y * tex_width)] : WHITE;
        color = WHITE;

        // Updated smooth version
        // float base_lighting_level = 1.0f
        //     - get_lighting_wall(
        //         (float)tex_x / tex_width, (float)tex_y * 0.0234375 /*(3.0f / tex_height)*/, pos, ray->wall.orientation);

        color = interpolate(color, BLACK, base_lighting_level) | ALPHA_OPAQUE_HEX;

        // DEBUGGING
        const int SCREEN_WIDTH = 1920;
        const int SCREEN_HEIGHT = 1080;

        if (current_x == SCREEN_WIDTH / 2 && y >= SCREEN_HEIGHT / 2 && y <= SCREEN_HEIGHT - 200) {
            color = RED;
        }

        dest_buffer_data->pixels[current_x + (y * buffer_width)] = color;
    }
}

/**
 * Writes the horizontal wall and ceiling strips to the pixel buffer.
 */
static void write_floor_and_celing(const Position* position, const Direction* direction, const CameraPlane* plane,
    PixelBuffer* dest_buffer_data, int screen_width, int screen_height, ht* texture_map)
{
    // Gets floor texture
    ht* floor_textures = (ht*)ht_get(texture_map, "floors");
    SDL_Surface* floor = (SDL_Surface*)ht_get(floor_textures, "1");
    Uint32* floor_pixels = floor != NULL ? floor->pixels : NULL;

    int floor_width = floor != NULL ? floor->w : TEXTURE_WIDTH_FALLBACK;
    int floor_height = floor != NULL ? floor->h : TEXTURE_HEIGHT_FALLBACK;

    // Gets ceiling texture
    ht* ceiling_textures = (ht*)ht_get(texture_map, "ceilings");
    SDL_Surface* ceiling = (SDL_Surface*)ht_get(ceiling_textures, "1");
    Uint32* ceiling_pixels = ceiling != NULL ? ceiling->pixels : NULL;

    int ceiling_width = ceiling != NULL ? ceiling->w : TEXTURE_WIDTH_FALLBACK;
    int ceiling_height = ceiling != NULL ? ceiling->h : TEXTURE_HEIGHT_FALLBACK;

    float ray_dir_x_0 = direction->x - plane->x;
    float ray_dir_y_0 = direction->y - plane->y;
    float ray_dir_x_1 = direction->x + plane->x;
    float ray_dir_y_1 = direction->y + plane->y;

    float pos_z = 0.5 * screen_height;

    int starting_y = screen_height / 2 + 1;

    for (int y = starting_y; y < screen_height; ++y) {
        int p = y - screen_height / 2;

        float row_distance = pos_z / p;

        float unit_step_x = row_distance * (ray_dir_x_1 - ray_dir_x_0) / screen_width;
        float unit_step_y = row_distance * (ray_dir_y_1 - ray_dir_y_0) / screen_width;

        float cell_pos_x = position->x + row_distance * ray_dir_x_0;
        float cell_pos_y = position->y + row_distance * ray_dir_y_0;

        for (int x = 0; x < screen_width; ++x) {
            // Pretty sure this is the current floor x and y equivalent
            int cell_x = (int)cell_pos_x;
            int cell_y = (int)cell_pos_y;

            int ftx = (int)(cell_pos_x * ceiling_width) % ceiling_width;
            int fty = (int)(cell_pos_y * ceiling_width) % ceiling_width;

            // Older version
            // float base_lighting_level = 1.0f - all_vertices[cell_x + (cell_y * COLS)];

            int texture_x = (int)(floor_width * (cell_pos_x - cell_x)) & (floor_width - 1);
            int texture_y = (int)(floor_height * (cell_pos_y - cell_y)) & (floor_height - 1);

            int location = cell_x + (cell_y * COLS);
            float light_val
                = 1.0f - get_lighting_floor((float)ftx / ceiling_width, (float)fty / ceiling_height, location);

            Uint32 colour;

            // Floor colour
            colour = floor_pixels[texture_x + (floor_width * texture_y)];
            colour = WHITE;

            // I can see from this that the values are still static, and not varying across the pixels in a cell
            // if (x == screen_width / 2 && y >= screen_height / 2 && y <= screen_height - 200) {
            //     colour = RED;
            // }

            // colour = interpolate(colour, BLACK, base_lighting_level) | ALPHA_OPAQUE_HEX;
            colour = interpolate(colour, BLACK, light_val) | ALPHA_OPAQUE_HEX;

            // If its the light source, draw in red
            if (light_map[cell_x + (cell_y * COLS)] >= 1.0f) {
                colour = RED;
            }

            dest_buffer_data->pixels[x + (y * screen_width)] = colour;

            // Recalculate texture x and y
            texture_x = (int)(ceiling_width * (cell_pos_x - cell_x)) & (ceiling_width - 1);
            texture_y = (int)(ceiling_height * (cell_pos_y - cell_y)) & (ceiling_height - 1);
            cell_pos_x += unit_step_x;
            cell_pos_y += unit_step_y;

            // Ceiling colour
            colour = ceiling_pixels[texture_x + (ceiling_width * texture_y)];
            colour = WHITE;

            colour = interpolate(colour, BLACK, light_val) | ALPHA_OPAQUE_HEX;
            dest_buffer_data->pixels[x + ((screen_height - y - 1) * screen_width)] = colour;
        }
    }
}

/**
 * Cleanup function. Frees resources.
 */
static void raycaster_cleanup(ecs_world_t* world, void* ctx)
{
    const char* z_buffer_name = "raycaster.systems.ZBuffer";
    ecs_entity_t z_buffer_entity = ecs_lookup(world, z_buffer_name);

    if (z_buffer_entity) {
        ZBuffer* z_buffer = ecs_get_mut(world, z_buffer_entity, ZBuffer);
        free(z_buffer->buffer);
        z_buffer->buffer = NULL;
        ecs_delete(world, z_buffer);
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not find z_buffer entity to delete with name: %s\n", z_buffer_name);
    }

    const char* pixel_buffer_name = "raycaster.systems.PixelBuffer";
    ecs_entity_t pixel_buffer_entity = ecs_lookup(world, pixel_buffer_name);

    if (pixel_buffer_entity) {
        PixelBuffer* pixel_buffer = ecs_get_mut(world, pixel_buffer_entity, PixelBuffer);
        free(pixel_buffer->pixels);
        pixel_buffer->pixels = NULL;
        ecs_delete(world, pixel_buffer);
    } else {
        SDL_LogError(
            SDL_LOG_CATEGORY_ERROR, "Could not find pixel_buffer entity to delete with name: %s\n", pixel_buffer_name);
    }
}

void RaycasterMapUpdate(ecs_iter_t* it)
{
    ecs_entity_t player = ecs_lookup(it->world, PLAYER_ENTITY_NAME);
    const Position* position = ecs_get(it->world, player, Position);
    const Direction* direction = ecs_get(it->world, player, Direction);
    const CameraPlane* plane = ecs_get(it->world, player, CameraPlane);

    ht* texture_map = ecs_field(it, Textures, 0)->table;
    double* z_buffer = ecs_field(it, ZBuffer, 1)->buffer;
    PixelBuffer* pixel_buffer = ecs_field(it, PixelBuffer, 2);

    // Clear the buffer
    memset(pixel_buffer->pixels, 0, pixel_buffer->size);

    // Write to the buffer
    int screen_width = pixel_buffer->width;
    int screen_height = pixel_buffer->height;

    write_floor_and_celing(position, direction, plane, pixel_buffer, screen_width, screen_height, texture_map);

    for (int x = 0; x < screen_width; x++) {
        Ray ray = { 0 };

        dda(position, direction, plane, x, screen_width, screen_height, &ray);
        write_vertical_wall_strip(&ray, position, x, pixel_buffer, texture_map);

        z_buffer[x] = ray.perp_wall_dist;
    }
}

void RaycasterSpriteUpdate(ecs_iter_t* it)
{
    Sprite* s = ecs_field(it, Sprite, 0);
    Position* p = ecs_field(it, Position, 1);
    ht* texture_map = ecs_field(it, Textures, 2)->table;
    const double* z_buffer = ecs_field(it, ZBuffer, 3)->buffer;
    PixelBuffer* buffer_data = ecs_field(it, PixelBuffer, 4);

    const VideoConfig* video_config = ecs_singleton_get(it->world, VideoConfig);

    // Use src.id in query instead
    const ecs_entity_t player = ecs_lookup(it->world, PLAYER_ENTITY_NAME);

    const Position* position = ecs_get(it->world, player, Position);
    const Direction* direction = ecs_get(it->world, player, Direction);
    const CameraPlane* plane = ecs_get(it->world, player, CameraPlane);

    int screen_width = video_config->screen_size.x;
    int screen_height = video_config->screen_size.y;

    // Sorts the sprites from far to close
    for (int i = 0; i < it->count; i++) {
        sprite_order[i] = i;
        sprite_distance[i]
            = ((position->x - p[i].x) * (position->x - p[i].x) + (position->y - p[i].y) * (position->y - p[i].y));
    }

    sort_sprites(sprite_order, sprite_distance, it->count);

    for (int i = 0; i < it->count; i++) {
        ecs_entity_t entity = it->entities[sprite_order[i]];
        p = ecs_get_mut(it->world, entity, Position);
        s = ecs_get_mut(it->world, entity, Sprite);

        double sprite_x = p->x - position->x;
        double sprite_y = p->y - position->y;

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

        ht* sprites = (ht*)ht_get(texture_map, "sprites");

        if (sprites == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sprites is null");
            return;
        }

        char buffer[BUFFER_MAX];
        SDL_itoa(s->sprite_id, buffer, 10);

        SDL_Surface* sprite_surface = (SDL_Surface*)ht_get(sprites, buffer);

        if (sprite_surface == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sprite %s surface is null", buffer);
            return;
        }

        Uint32* pixels = (Uint32*)sprite_surface->pixels;

        for (int stripe = draw_start_x; stripe < draw_end_x; stripe++) {
            int tex_x = (int)(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) * sprite_surface->w / sprite_width)
                / 256;

            if (transform_y > 0 && stripe > 0 && stripe < screen_width && transform_y < z_buffer[stripe]) {

                for (int y = draw_start_y; y < draw_end_y; y++) {
                    int d = (y) * 256 - screen_height * 128 + sprite_height * 128;

                    int tex_y = ((d * sprite_surface->h) / sprite_height) / 256;

                    Uint32 color = pixels[sprite_surface->w * tex_y + tex_x];

                    if ((color & 0x00FFFFFF) != 0) {
                        buffer_data->pixels[stripe + (y * buffer_data->width)] = color;
                    }
                }
            }
        }
    }
}

void RaycasterBlitBufferToTexture(ecs_iter_t* it)
{
    ScreenTexture* screen_texture = ecs_field(it, ScreenTexture, 0);
    PixelBuffer* pixel_buffer = ecs_field(it, PixelBuffer, 1);

    void* buffer = NULL;
    int texture_pitch = pixel_buffer->width * sizeof(Uint32);

    if (!SDL_LockTexture(screen_texture->texture, NULL, &buffer, &texture_pitch)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error locking pixels texture.");
        return;
    }

    memcpy(buffer, pixel_buffer->pixels, pixel_buffer->size);

    SDL_UnlockTexture(screen_texture->texture);
}

void RaycasterDrawWorld(ecs_iter_t* it)
{
    ScreenTexture* screen_texture = ecs_field(it, ScreenTexture, 0);
    SDL_Renderer* renderer = ecs_singleton_get(it->world, Renderer)->value;
    SDL_Window* window = ecs_singleton_get(it->world, Window)->value;

    SDL_RenderTexture(renderer, screen_texture->texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

void RaycasterSystemsImport(ecs_world_t* world)
{
    ECS_MODULE(world, RaycasterSystems);

    ECS_IMPORT(world, RaycasterComponents);
    ECS_IMPORT(world, TransformComponents);
    ECS_IMPORT(world, GameManagerComponents);

    const VideoConfig* video_config = ecs_singleton_get(world, VideoConfig);

    int screen_width = video_config->screen_size.x;
    int screen_height = video_config->screen_size.y;

    DRAW_START_MIN = 0;
    DRAW_END_MAX = screen_height;

    const Renderer* renderer = ecs_singleton_get(world, Renderer);

    SDL_Texture* screen_texture = SDL_CreateTexture(
        renderer->value, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);

    if (screen_texture == NULL) {
        SDL_Log("Unabled to create pixels texture: %s\n", SDL_GetError());
        return;
    }

    ecs_entity_t screen = ecs_new(world);
    ecs_set(world, screen, ScreenTexture, { screen_texture });

    PixelBuffer buffer_data = { 0 };
    buffer_data.pixels = (Uint32*)malloc(screen_width * screen_height * sizeof(Uint32));
    buffer_data.width = screen_width;
    buffer_data.height = screen_height;
    buffer_data.size = screen_width * screen_height * sizeof(Uint32);

    ecs_entity_t pixel_buffer = ecs_entity(world, { .name = "PixelBuffer" });
    ecs_add(world, pixel_buffer, PixelBuffer);
    ecs_set_ptr(world, pixel_buffer, PixelBuffer, &buffer_data);

    ecs_entity_t z_buffer = ecs_entity(world, { .name = "ZBuffer" });
    ecs_set(world, z_buffer, ZBuffer, { (double*)calloc(video_config->screen_size.x, sizeof(double)) });

    const TexturesConfig* textures_config = ecs_singleton_get(world, TexturesConfig);

    ecs_entity_t textures = ecs_new(world);
    ecs_set(world, textures, Textures, { create_textures_from_config(textures_config->config) });

    ecs_entity_t RaycasterMap = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t RaycasterSprite = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t RaycasterBlit = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t RaycasterDraw = ecs_new_w_id(world, EcsPhase);

    ecs_add_pair(world, RaycasterMap, EcsDependsOn, EcsOnUpdate);
    ecs_add_pair(world, RaycasterSprite, EcsDependsOn, RaycasterMap);
    ecs_add_pair(world, RaycasterBlit, EcsDependsOn, RaycasterSprite);
    ecs_add_pair(world, RaycasterDraw, EcsDependsOn, RaycasterBlit);

    ecs_system(world,
        { .entity = ecs_entity(world, { .name = "RaycasterMapUpdate", .add = ecs_ids(ecs_dependson(RaycasterMap)) }),
            .query.terms = { { ecs_id(Textures), .src.id = textures, .inout = EcsIn },
                { ecs_id(ZBuffer), .src.id = z_buffer, .inout = EcsInOut },
                { ecs_id(PixelBuffer), .src.id = pixel_buffer, .inout = EcsInOut } },
            .callback = RaycasterMapUpdate });

    ecs_system(world,
        { .entity
            = ecs_entity(world, { .name = "RaycasterSpriteUpdate", .add = ecs_ids(ecs_dependson(RaycasterSprite)) }),
            .query.terms = { { ecs_id(Sprite), .inout = EcsIn }, { ecs_id(Position), .inout = EcsIn },
                { ecs_id(Textures), .src.id = textures, .inout = EcsIn },
                { ecs_id(ZBuffer), .src.id = z_buffer, .inout = EcsInOut },
                { ecs_id(PixelBuffer), .src.id = pixel_buffer, .inout = EcsInOut } },
            .callback = RaycasterSpriteUpdate });

    ecs_system(world,
        { .entity = ecs_entity(
              world, { .name = "RaycasterBlitBufferToTexture", .add = ecs_ids(ecs_dependson(RaycasterBlit)) }),
            .query.terms = { { ecs_id(ScreenTexture), .src.id = screen, .inout = EcsInOut },
                { ecs_id(PixelBuffer), .src.id = pixel_buffer, .inout = EcsInOut } },
            .callback = RaycasterBlitBufferToTexture });

    ecs_system(world,
        { .entity = ecs_entity(world, { .name = "RaycasterDrawWorld", .add = ecs_ids(ecs_dependson(RaycasterDraw)) }),
            .query.terms = { { ecs_id(ScreenTexture), .src.id = screen, .inout = EcsIn } },
            .callback = RaycasterDrawWorld });

    ecs_atfini(world, raycaster_cleanup, NULL);
}