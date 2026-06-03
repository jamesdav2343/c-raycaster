#include "systems/game_manager.h"
#include "general_utils.h"

static void initialise_entities(ecs_world_t* world, cJSON* entities_json)
{
    const cJSON* element = NULL;

    cJSON_ArrayForEach(element, entities_json)
    {
        ecs_entity_t entity = ecs_new(world);

        // Tag
        const char* tag = cJSON_GetObjectItem(element, "tag")->valuestring;
        // ECS_TAG(world, $tag);
        ecs_entity_desc_t desc = { 0 };
        desc.id = entity;
        desc.name = tag;
        desc.add_expr = "0";

        ecs_entity_init(world, &desc);

        // Position
        cJSON* position = cJSON_GetObjectItem(element, "position");
        double x = cJSON_GetObjectItem(position, "x")->valuedouble;
        double y = cJSON_GetObjectItem(position, "y")->valuedouble;

        ecs_set(world, entity, Position, { (float)x, (float)y });

        // Sprite Id
        int sprite_id = cJSON_GetObjectItem(element, "sprite_id")->valueint;
        ecs_set(world, entity, Sprite, { sprite_id });
    }
}

static void initialise_textures_table(ht* textures_table, cJSON* textures_json, const char* texture_type)
{
    const cJSON* element = NULL;

    cJSON_ArrayForEach(element, cJSON_GetObjectItem(textures_json, texture_type))
    {
        const char* path = cJSON_GetObjectItem(element, "path")->valuestring;
        int width = cJSON_GetObjectItem(element, "width")->valueint;
        int height = cJSON_GetObjectItem(element, "height")->valueint;

        TextureData* texture_data = (TextureData*)malloc(sizeof(TextureData));
        // TODO: Free this string in cleanup
        texture_data->path = strdup(path);
        texture_data->size = (Vector2I) { width, height };

        ht_set(textures_table, element->string, texture_data);
    };
}

static void set_config(ecs_world_t* world)
{
    // -- Config loading --

    // VIDEO
    cJSON* json = load_config_json(CONFIG_FILE);
    cJSON* video_config = cJSON_GetObjectItem(json, "video");

    cJSON* resolution = cJSON_GetObjectItem(video_config, "resolution");
    cJSON* screen_width = cJSON_GetObjectItem(resolution, "width");
    cJSON* screen_height = cJSON_GetObjectItem(resolution, "height");

    cJSON* fps_cap = cJSON_GetObjectItem(video_config, "fps_cap");
    cJSON* enable_lighting = cJSON_GetObjectItem(video_config, "enable_lighting");

    ecs_singleton_set(world, VideoConfig,
        { { screen_width->valueint, screen_height->valueint }, fps_cap->valueint, enable_lighting->valueint });

    // TEXTURES
    cJSON* textures_json = cJSON_GetObjectItem(json, "textures");
    ht* textures_config = ht_create();

    // Walls
    ht* walls = ht_create();
    initialise_textures_table(walls, textures_json, "walls");
    ht_set(textures_config, "walls", walls);

    // Ceilings
    ht* ceilings = ht_create();
    initialise_textures_table(ceilings, textures_json, "ceilings");
    ht_set(textures_config, "ceilings", ceilings);

    // Floors
    ht* floors = ht_create();
    initialise_textures_table(floors, textures_json, "floors");
    ht_set(textures_config, "floors", floors);

    // Sprites
    ht* sprites = ht_create();
    initialise_textures_table(sprites, textures_json, "sprites");
    ht_set(textures_config, "sprites", sprites);

    ecs_singleton_set(world, TexturesConfig, { textures_config });

    // ENTITIES
    cJSON* entities_json = cJSON_GetObjectItem(json, "entities");
    initialise_entities(world, entities_json);

    cJSON_Delete(json);
}

void GameManagerSystemsImport(ecs_world_t* world)
{
    ECS_IMPORT(world, GameManagerComponents);
    ECS_IMPORT(world, TransformComponents);
    ECS_IMPORT(world, SpriteComponents);

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
    ht* textures_config = ecs_singleton_get(world, TexturesConfig)->config;

    ht* walls = (ht*)ht_get(textures_config, "walls");
    ht* ceilings = (ht*)ht_get(textures_config, "ceilings");
    ht* floors = (ht*)ht_get(textures_config, "floors");

    if (walls != NULL)
        ht_destroy(walls);

    if (ceilings != NULL)
        ht_destroy(ceilings);

    if (floors != NULL)
        ht_destroy(floors);

    if (textures_config != NULL)
        ht_destroy(textures_config);

    SDL_Renderer* renderer = ecs_singleton_get(world, Renderer)->value;
    SDL_Window* window = ecs_singleton_get(world, Window)->value;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Game closed.\n");
}