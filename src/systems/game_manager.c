#include "systems/game_manager.h"
#include "general_utils.h"

static void set_config(ecs_world_t* world)
{
    // -- Config loading --

    // Video
    cJSON* json = load_config_json(CONFIG_FILE);
    cJSON* video_config = cJSON_GetObjectItem(json, "video");

    cJSON* resolution = cJSON_GetObjectItem(video_config, "resolution");
    cJSON* screen_width = cJSON_GetObjectItem(resolution, "width");
    cJSON* screen_height = cJSON_GetObjectItem(resolution, "height");

    cJSON* fps_cap = cJSON_GetObjectItem(video_config, "fps_cap");
    cJSON* enable_lighting = cJSON_GetObjectItem(video_config, "enable_lighting");

    ecs_singleton_set(world, VideoConfig,
        { { screen_width->valueint, screen_height->valueint }, fps_cap->valueint, enable_lighting->valueint });

    // Textures
    cJSON* textures_json = cJSON_GetObjectItem(json, "textures");
    ht* textures = ht_create();

    // Walls
    ht* walls = ht_create();
    const cJSON* wall = NULL;

    cJSON_ArrayForEach(wall, cJSON_GetObjectItem(textures_json, "walls"))
    {
        const char* path = cJSON_GetObjectItem(wall, "path")->valuestring;
        int width = cJSON_GetObjectItem(wall, "width")->valueint;
        int height = cJSON_GetObjectItem(wall, "height")->valueint;

        TextureData* texture_data = (TextureData*)malloc(sizeof(TextureData));
        texture_data->path = path;
        texture_data->size = (Vector2I) { width, height };

        ht_set(walls, wall->string, texture_data);
    };

    ht_set(textures, "walls", walls);

    // Ceilings
    ht* ceilings = ht_create();
    const cJSON* ceiling = NULL;

    cJSON_ArrayForEach(ceiling, cJSON_GetObjectItem(textures_json, "ceilings"))
    {
        const char* path = cJSON_GetObjectItem(ceiling, "path")->valuestring;
        int width = cJSON_GetObjectItem(ceiling, "width")->valueint;
        int height = cJSON_GetObjectItem(ceiling, "height")->valueint;

        TextureData* texture_data = (TextureData*)malloc(sizeof(TextureData));
        texture_data->path = path;
        texture_data->size = (Vector2I) { width, height };

        ht_set(ceilings, ceiling->string, texture_data);
    };

    ht_set(textures, "ceilings", ceilings);

    // Floors
    ht* floors = ht_create();
    const cJSON* floor = NULL;

    cJSON_ArrayForEach(floor, cJSON_GetObjectItem(textures_json, "floors"))
    {
        const char* path = cJSON_GetObjectItem(floor, "path")->valuestring;
        int width = cJSON_GetObjectItem(floor, "width")->valueint;
        int height = cJSON_GetObjectItem(floor, "height")->valueint;

        TextureData* texture_data = (TextureData*)malloc(sizeof(TextureData));
        texture_data->path = path;
        texture_data->size = (Vector2I) { width, height };

        ht_set(floors, floor->string, texture_data);
    };

    ht_set(textures, "floors", floors);

    ecs_singleton_set(world, TexturesConfig, { textures });
    cJSON_Delete(json);
}

void GameManagerSystemsImport(ecs_world_t* world)
{
    ECS_IMPORT(world, GameManagerComponents);

    set_config(world);

    ECS_MODULE(world, GameManagerSystems);

    SDL_Window* window;
    SDL_Renderer* renderer;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }

    const VideoConfig* video_config = ecs_singleton_get(world, VideoConfig);

    window = SDL_CreateWindow(TITLE, video_config->screen_size.x, video_config->screen_size.y, SDL_WINDOW_RESIZABLE);

    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        return;
    }

    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    renderer = SDL_CreateSoftwareRenderer(window_surface);

    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create software renderer: %s", SDL_GetError());
        return;
    }

    ecs_add_id(world, ecs_id(Window), EcsSingleton);
    ecs_singleton_set(world, Window, { window });

    ecs_add_id(world, ecs_id(Renderer), EcsSingleton);
    ecs_singleton_set(world, Renderer, { renderer });

    ecs_atfini(world, game_manager_cleanup, NULL);
}

void handle_events(SDL_Event* event, GameStatus* game_status)
{
    SDL_PollEvent(event);

    if (event->type == SDL_EVENT_QUIT) {
        game_status->is_running = false;
    }
}

void game_manager_cleanup(ecs_world_t* world, void* ctx)
{
    SDL_Renderer* renderer = ecs_singleton_get(world, Renderer)->value;
    SDL_Window* window = ecs_singleton_get(world, Window)->value;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Game closed.\n");
}