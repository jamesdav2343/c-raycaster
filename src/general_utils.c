#include "general_utils.h"
#include "types.h"
#include <SDL3_image/SDL_image.h>
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

ht* create_textures_from_config(ht* config, size_t num_textures)
{
    ht* textures = ht_create();

    ht* walls = (ht*)ht_get(config, "walls");
    ht* ceilings = (ht*)ht_get(config, "ceilings");
    ht* floors = (ht*)ht_get(config, "floors");

    ht* walls_textures = ht_create();
    ht* ceilings_textures = ht_create();
    ht* floors_textures = ht_create();

    hti it;

    it = ht_iterator(walls);
    while (ht_next(&it)) {
        ht_set(walls_textures, it.key,
            load_formatted_img_surface(((TextureData*)it.value)->path, SDL_PIXELFORMAT_ARGB8888));
    }
    ht_set(textures, "walls", walls_textures);

    it = ht_iterator(ceilings);
    while (ht_next(&it)) {
        ht_set(ceilings_textures, it.key,
            load_formatted_img_surface(((TextureData*)it.value)->path, SDL_PIXELFORMAT_ARGB8888));
    }
    ht_set(textures, "ceilings", ceilings_textures);

    it = ht_iterator(floors);
    while (ht_next(&it)) {
        ht_set(floors_textures, it.key,
            load_formatted_img_surface(((TextureData*)it.value)->path, SDL_PIXELFORMAT_ARGB8888));
    }
    ht_set(textures, "floors", floors_textures);

    return textures;
}