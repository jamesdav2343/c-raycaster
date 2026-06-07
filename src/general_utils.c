#include "general_utils.h"
#include "map.h"
#include "types.h"
#include <SDL3_image/SDL_image.h>
#include <limits.h>
#include <stdlib.h>

Uint32 interpolate(int color1, int color2, float fraction)
{
    unsigned char r1 = (color1 >> 16) & 0xff;
    unsigned char r2 = (color2 >> 16) & 0xff;
    unsigned char g1 = (color1 >> 8) & 0xff;
    unsigned char g2 = (color2 >> 8) & 0xff;
    unsigned char b1 = color1 & 0xff;
    unsigned char b2 = color2 & 0xff;

    return (int)((r2 - r1) * fraction + r1) << 16 | (int)((g2 - g1) * fraction + g1) << 8
        | (int)((b2 - b1) * fraction + b1);
}

// ---- Config loading helpers ----
cJSON* load_config_json(const char* path)
{
    cJSON* json = NULL;

    char* buffer = 0;
    size_t length;
    FILE* f = fopen(path, "rb");

    if (f == NULL) {
        fprintf(stderr, "Unable to open json file: %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = malloc(length);

    if (buffer) {
        fread(buffer, 1, length, f);
        fclose(f);
    } else {
        fprintf(stderr, "Could not read json from file.\n");
        return NULL;
    }

    json = cJSON_Parse(buffer);
    free(buffer);

    if (json == NULL) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        return NULL;
    }

    return json;
}

SDL_Surface* load_formatted_img_surface(const char* img_path, SDL_PixelFormat format)
{
    SDL_Surface* img_surface = IMG_Load(img_path);
    SDL_Surface* formatted_surface = SDL_ConvertSurface(img_surface, format);
    SDL_DestroySurface(img_surface);

    return formatted_surface;
}

ht* create_textures_from_config(ht* config)
{
    ht* textures = ht_create();

    ht* walls = (ht*)ht_get(config, "walls");
    ht* ceilings = (ht*)ht_get(config, "ceilings");
    ht* floors = (ht*)ht_get(config, "floors");
    ht* sprites = (ht*)ht_get(config, "sprites");

    ht* walls_textures = ht_create();
    ht* ceilings_textures = ht_create();
    ht* floors_textures = ht_create();
    ht* sprites_textures = ht_create();

    hti it;

    it = ht_iterator(walls);
    while (ht_next(&it)) {
        ht_set(walls_textures, it.key, load_formatted_img_surface(it.value, SDL_PIXELFORMAT_ARGB8888));
    }
    ht_set(textures, "walls", walls_textures);

    it = ht_iterator(ceilings);
    while (ht_next(&it)) {
        ht_set(ceilings_textures, it.key, load_formatted_img_surface(it.value, SDL_PIXELFORMAT_ARGB8888));
    }
    ht_set(textures, "ceilings", ceilings_textures);

    it = ht_iterator(floors);
    while (ht_next(&it)) {
        ht_set(floors_textures, it.key, load_formatted_img_surface(it.value, SDL_PIXELFORMAT_ARGB8888));
    }
    ht_set(textures, "floors", floors_textures);

    it = ht_iterator(sprites);
    while (ht_next(&it)) {
        ht_set(sprites_textures, it.key, load_formatted_img_surface(it.value, SDL_PIXELFORMAT_ARGB8888));
    }
    ht_set(textures, "sprites", sprites_textures);

    return textures;
}

// ---- Sprite casting helpers ----

// Comparison function for qsort (ascending order)
int compare_sprites(const void* a, const void* b)
{
    const SpriteSortPair* p1 = (const SpriteSortPair*)a;
    const SpriteSortPair* p2 = (const SpriteSortPair*)b;

    if (p1->distance < p2->distance)
        return -1;
    if (p1->distance > p2->distance)
        return 1;
    return 0;
}

void sort_sprites(int* order, double* distance, int amount)
{
    if (amount <= 0)
        return;

    // Allocate temporary array for sorting
    SpriteSortPair* sprites = malloc(sizeof(SpriteSortPair) * amount);
    if (!sprites)
        return; // Handle allocation failure

    for (int i = 0; i < amount; i++) {
        sprites[i].distance = distance[i];
        sprites[i].order = order[i];
    }

    // Sort the array in ascending order
    qsort(sprites, amount, sizeof(SpriteSortPair), compare_sprites);

    // Restore in reverse order (farthest to nearest)
    for (int i = 0; i < amount; i++) {
        distance[i] = sprites[amount - i - 1].distance;
        order[i] = sprites[amount - i - 1].order;
    }

    free(sprites);
}

static int num_places(int n)
{
    if (n < 0)
        return num_places((n == INT_MIN) ? INT_MAX : -n);
    if (n < 10)
        return 1;
    return 1 + num_places(n / 10);
}

void pretty_print_grid(Uint8* grid, int row_length, int max_gap)
{
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            Uint8 val = grid[x + (y * COLS)];
            Uint8 num_digits = (Uint8)num_places(val);

            char grid_item[BUFFER_MAX] = "";
            char space[] = " ";

            for (int i = 0; i < max_gap - num_digits; i++) {
                strncat(grid_item, space, BUFFER_MAX);
            }

            char val_buffer[BUFFER_MAX];
            snprintf(val_buffer, BUFFER_MAX, "%d", val);
            strncat(grid_item, val_buffer, BUFFER_MAX);

            printf("%s, ", grid_item);
        }
        printf("\n");
    }
}