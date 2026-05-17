#include "general_utils.h"
#include "systems/raycaster.h"

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

typedef struct {
    int version;
    const char* name;
    const char* email;
} configuration;

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

    // char* string = cJSON_Print(json);
    // printf("\n%s\n", string);

    // Uint32* textures[8];

    // for (int i = 0; i < 8; i++) {
    //     textures[i] = (Uint32*)malloc(TEXTURE_HEIGHT * TEXTURE_WIDTH * sizeof(Uint32));

    //     if (textures[i] == NULL) {
    //         SDL_Log("Unable to init textures.");
    //         return;
    //     }
    // }

    // SDL_Surface* wall = IMG_Load("assets/Wall_1.png");
    // SDL_Surface* wall_1 = SDL_ConvertSurface(wall, SDL_PIXELFORMAT_ARGB8888);
    // SDL_DestroySurface(wall);

    // SDL_Surface* floor = IMG_Load("assets/Floor_1.png");
    // SDL_Surface* floor_1 = SDL_ConvertSurface(floor, SDL_PIXELFORMAT_ARGB8888);
    // SDL_DestroySurface(floor);

    // SDL_Surface* ceiling = IMG_Load("assets/Ceiling_1.png");
    // SDL_Surface* ceiling_1 = SDL_ConvertSurface(ceiling, SDL_PIXELFORMAT_ARGB8888);
    // SDL_DestroySurface(ceiling);

    // SDL_Surface* pillar = IMG_Load("test/pillar.png");
    // SDL_Surface* pillar_1 = SDL_ConvertSurface(pillar, SDL_PIXELFORMAT_ARGB8888);
    // SDL_DestroySurface(pillar);

    // SDL_Surface* light = IMG_Load("test/greenlight.png");
    // SDL_Surface* light_1 = SDL_ConvertSurface(light, SDL_PIXELFORMAT_ABGR8888);
    // SDL_DestroySurface(light);

    // textures[0] = (Uint32*)wall_1->pixels;
    // textures[1] = (Uint32*)floor_1->pixels;
    // textures[2] = (Uint32*)ceiling_1->pixels;

    // textures[3] = (Uint32*)pillar_1->pixels;
    // textures[4] = (Uint32*)light_1->pixels;
}