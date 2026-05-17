#include "systems/game_manager.h"
#include "general_utils.h"

void GameManagerSystemsImport(ecs_world_t* world)
{
    ECS_IMPORT(world, GameManagerComponents);

    // Config loading
    cJSON* json = load_config_json(CONFIG_FILE);
    cJSON* video_config = cJSON_GetObjectItem(json, "video");

    cJSON* resolution = cJSON_GetObjectItem(video_config, "resolution");
    cJSON* screen_width = cJSON_GetObjectItem(resolution, "width");
    cJSON* screen_height = cJSON_GetObjectItem(resolution, "height");

    cJSON* fps_cap = cJSON_GetObjectItem(video_config, "fps_cap");

    ecs_singleton_set(world, VideoConfig, { { screen_width->valueint, screen_height->valueint }, fps_cap->valueint });

    printf("%s\n", cJSON_Print(json));

    printf("%d\n", ecs_singleton_get(world, VideoConfig)->fps_cap);

    ECS_MODULE(world, GameManagerSystems);

    SDL_Window* window;
    SDL_Renderer* renderer;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }

    window = SDL_CreateWindow(TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);

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

    cJSON_Delete(json);
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