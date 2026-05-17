#include "components/game_manager.h"

ECS_COMPONENT_DECLARE(Window);
ECS_COMPONENT_DECLARE(Renderer);
ECS_COMPONENT_DECLARE(VideoConfig);
ECS_COMPONENT_DECLARE(TexturesConfig);
ECS_TAG_DECLARE(Config);

void GameManagerComponentsImport(ecs_world_t* world)
{
    ECS_COMPONENT_DEFINE(world, Window);
    ECS_COMPONENT_DEFINE(world, Renderer);
    ECS_COMPONENT_DEFINE(world, VideoConfig);
    ECS_COMPONENT_DEFINE(world, TexturesConfig);
    ECS_TAG_DEFINE(world, Config);

    ECS_MODULE(world, GameManagerComponents);
}