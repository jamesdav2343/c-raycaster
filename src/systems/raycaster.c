#include "systems/raycaster.h"
#include "components/game_manager.h"
#include "types.h"

ECS_SYSTEM_DECLARE(RaycasterUpdate);
ECS_SYSTEM_DECLARE(RaycasterDestroy);
ECS_SYSTEM_DECLARE(RaycasterDraw);

#define TEXTURE_WIDTH 128
#define TEXTURE_HEIGHT 128

static SDL_Texture* pixels_texture;

// Convert this to a map of wall id to array
static Uint32* textures[8];

static int DRAW_START_MIN;
static int DRAW_END_MAX;

// SPRITE STUFF
#define numSprites 19

Sprite sprite[numSprites] = { { 12, 14, 3 }, // pillar
    { 12, 15, 4 } }; // light

static double* ZBuffer = NULL;

// arrays used to sort the sprites
int spriteOrder[numSprites];
double spriteDistance[numSprites];

// function used to sort the sprites
typedef struct {
    double dist;
    int order;
} SpriteSortPair;

// Comparison function for qsort (ascending order)
static int compareSprites(const void* a, const void* b)
{
    const SpriteSortPair* p1 = (const SpriteSortPair*)a;
    const SpriteSortPair* p2 = (const SpriteSortPair*)b;

    if (p1->dist < p2->dist)
        return -1;
    if (p1->dist > p2->dist)
        return 1;
    return 0;
}

static void sortSprites(int* order, double* dist, int amount)
{
    if (amount <= 0)
        return;

    // Allocate temporary array for sorting
    SpriteSortPair* sprites = malloc(sizeof(SpriteSortPair) * amount);
    if (!sprites)
        return; // Handle allocation failure

    for (int i = 0; i < amount; i++) {
        sprites[i].dist = dist[i];
        sprites[i].order = order[i];
    }

    // Sort the array in ascending order
    qsort(sprites, amount, sizeof(SpriteSortPair), compareSprites);

    // Restore in reverse order (farthest to nearest)
    for (int i = 0; i < amount; i++) {
        dist[i] = sprites[amount - i - 1].dist;
        order[i] = sprites[amount - i - 1].order;
    }

    free(sprites);
}

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

/*
Writes a vertical wall strip to the pixel buffer.
*/
static void write_vertical_wall_strip(Ray* ray, const Position* position, int current_x, PixelBuffer* dest_buffer_data)
{
    int buffer_height = dest_buffer_data->height;
    int buffer_width = dest_buffer_data->width;

    int line_height = (int)(buffer_height / ray->perp_wall_dist);
    int draw_start = (int)fmax(-line_height / 2 + buffer_height / 2, DRAW_START_MIN);
    int draw_end = (int)fmin(line_height / 2 + buffer_height / 2, DRAW_END_MAX);

    // calculate value of wallX
    double wall_x = ray->wall.side_orientation == HORIZONTAL ? position->y + ray->perp_wall_dist * ray->direction.y
                                                             : position->x + ray->perp_wall_dist * ray->direction.x;

    wall_x -= floor(wall_x);

    // x coordinate on the texture
    int tex_x = (int)(wall_x * (double)TEXTURE_WIDTH);

    if (ray->wall.side_orientation == HORIZONTAL && ray->direction.x > 0)
        tex_x = TEXTURE_WIDTH - tex_x - 1;

    if (ray->wall.side_orientation == VERTICAL && ray->direction.y < 0)
        tex_x = TEXTURE_WIDTH - tex_x - 1;

    double step = 1.0 * TEXTURE_HEIGHT / line_height;

    // Starting texture coordinate
    double texture_coord = (draw_start - buffer_height / 2 + line_height / 2) * step;

    float base_lighting_level = 1.0f
        - get_wall_light_intensity(
            ray->wall.wall_position.x, ray->wall.wall_position.y, ray->direction, ray->wall.side_orientation);

    for (int y = draw_start; y < draw_end; y++) {
        // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
        int tex_y = (int)texture_coord & (TEXTURE_HEIGHT - 1);
        texture_coord += step;

        Uint32 color = textures[0][TEXTURE_HEIGHT * tex_y + tex_x];

        // debugging
        color = 0xFFFFFFFF;

        color = interpolate(color, BLACK, base_lighting_level) | ALPHA_OPAQUE_HEX;

        dest_buffer_data->pixels[current_x + (y * buffer_width)] = color;
    }
}

/*
Casts a ray.
Uses the digital differential analyser (DDA) algorithm.
Great explanation of the algorithm here:
https://aaaa.sh/creatures/dda-algorithm-interactive/
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

/*
Writes the horizontal wall and ceiling strips to the pixel buffer.
*/
static void write_floor_and_celing(const Position* position, const Direction* direction, const Plane* plane,
    PixelBuffer* dest_buffer_data, int screen_width, int screen_height)
{
    float ray_dir_x_0 = direction->x - plane->x;
    float ray_dir_y_0 = direction->y - plane->y;
    float ray_dir_x_1 = direction->x + plane->x;
    float ray_dir_y_1 = direction->y + plane->y;

    float pos_z = 0.5 * screen_height;

    int starting_y = screen_height / 2 + 1;

    // Floor casting
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

            int texture_x = (int)(TEXTURE_WIDTH * (floor_x - cell_x)) & (TEXTURE_WIDTH - 1);
            int texture_y = (int)(TEXTURE_HEIGHT * (floor_y - cell_y)) & (TEXTURE_HEIGHT - 1);

            floor_x += floor_step_x;
            floor_y += floor_step_y;

            Uint32 colour;

            // Floor colour
            colour = textures[1][TEXTURE_WIDTH * texture_y + texture_x];

            // debugging
            colour = 0xFFFFFFFF;

            // If its the light source, draw in red
            if (light_map[cell_x + (cell_y * COLS)] >= 1.0f) {
                colour = 0xFFFF0000;
            }

            colour = interpolate(colour, BLACK, base_lighting_level) | ALPHA_OPAQUE_HEX;

            dest_buffer_data->pixels[x + (y * screen_width)] = colour;

            // Ceiling colour
            colour = textures[2][TEXTURE_WIDTH * texture_y + texture_x];

            // debugging
            colour = 0xFFFFFFFF;

            colour = interpolate(colour, BLACK, base_lighting_level) | ALPHA_OPAQUE_HEX;
            dest_buffer_data->pixels[x + ((screen_height - y - 1) * screen_width)] = colour;
        }
    }
}

static void write_to_buffer(
    const Position* position, const Direction* direction, const Plane* plane, PixelBuffer* dest_pixel_buffer)
{
    int screen_width = dest_pixel_buffer->width;
    int screen_height = dest_pixel_buffer->height;

    write_floor_and_celing(position, direction, plane, dest_pixel_buffer, screen_width, screen_height);

    // Wall casting for the current frame
    for (int x = 0; x < screen_width; x++) {
        Ray ray = { 0 };

        dda(position, direction, plane, x, screen_width, screen_height, &ray);
        write_vertical_wall_strip(&ray, position, x, dest_pixel_buffer);

        ZBuffer[x] = ray.perp_wall_dist;
    }

    // SPRITE CASTING
    /**
     * foreach sprite
     * draw the sprite similar to how walls and floors are drawn
     */

    for (int i = 0; i < numSprites; i++) {
        spriteOrder[i] = i;
        spriteDistance[i] = ((position->x - sprite[i].x) * (position->x - sprite[i].x)
            + (position->y - sprite[i].y) * (position->y - sprite[i].y)); // sqrt not taken, unneeded
    }

    sortSprites(spriteOrder, spriteDistance, numSprites);

    // after sorting the sprites, do the projection and draw them
    for (int i = 0; i < numSprites; i++) {
        // translate sprite position to relative to camera
        double spriteX = sprite[spriteOrder[i]].x - position->x;
        double spriteY = sprite[spriteOrder[i]].y - position->y;

        // transform sprite with the inverse camera matrix
        //  [ planeX   dirX ] -1                                       [ dirY      -dirX ]
        //  [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
        //  [ planeY   dirY ]                                          [ -planeY  planeX ]

        double invDet
            = 1.0 / (plane->x * direction->y - direction->x * plane->y); // required for correct matrix multiplication

        double transformX = invDet * (direction->y * spriteX - direction->x * spriteY);
        double transformY = invDet
            * (-plane->y * spriteX
                + plane->x * spriteY); // this is actually the depth inside the screen, that what Z is in 3D

        int spriteScreenX = (int)((screen_width / 2) * (1 + transformX / transformY));

        // calculate height of the sprite on screen
        int spriteHeight = abs(
            (int)(screen_height / (transformY))); // using 'transformY' instead of the real distance prevents fisheye
        // calculate lowest and highest pixel to fill in current stripe
        int drawStartY = -spriteHeight / 2 + screen_height / 2;
        if (drawStartY < 0)
            drawStartY = 0;
        int drawEndY = spriteHeight / 2 + screen_height / 2;
        if (drawEndY >= screen_height)
            drawEndY = screen_height - 1;

        // calculate width of the sprite
        int spriteWidth = abs((int)(screen_height / (transformY)));
        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        if (drawStartX < 0)
            drawStartX = 0;
        int drawEndX = spriteWidth / 2 + spriteScreenX;
        if (drawEndX >= screen_width)
            drawEndX = screen_width - 1;

        // loop through every vertical stripe of the sprite on screen
        for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
            int texX
                = (int)(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * SPRITE_TEXTURE_WIDTH / spriteWidth) / 256;
            // the conditions in the if are:
            // 1) it's in front of camera plane so you don't see things behind you
            // 2) it's on the screen (left)
            // 3) it's on the screen (right)
            // 4) ZBuffer, with perpendicular distance
            if (transformY > 0 && stripe > 0 && stripe < screen_width && transformY < ZBuffer[stripe]) {
                for (int y = drawStartY; y < drawEndY; y++) // for every pixel of the current stripe
                {
                    int d = (y) * 256 - screen_height * 128 + spriteHeight * 128; // 256 and 128 factors to avoid floats
                    int texY = ((d * SPRITE_TEXTURE_HEIGHT) / spriteHeight) / 256;
                    Uint32 color = textures[sprite[spriteOrder[i]].texture]
                                           [SPRITE_TEXTURE_WIDTH * texY + texX]; // get current color from the texture
                    if ((color & 0x00FFFFFF) != 0) {
                        dest_pixel_buffer->pixels[stripe + (y * dest_pixel_buffer->width)]
                            = color; // paint pixel if it isn't black, black is the invisible color
                    }
                }
            }
        }
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

    ZBuffer = (double*)calloc(video_config->screen_size.x, sizeof(double));

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

    create_textures_from_config(textures_config, numSprites);
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

void raycaster_cleanup(ecs_world_t* world, void* ctx)
{
    if (ZBuffer != NULL) {
        free(ZBuffer);
        ZBuffer = NULL;
    }

    const PixelBuffer* pixel_buffer = ecs_get(world, ecs_id(PixelBuffer), PixelBuffer);

    if (pixel_buffer->pixels != NULL) {
        free(pixel_buffer->pixels);
    }
}