#include "sprite.h"

ECS_COMPONENT_DECLARE(Sprite);

void SpriteModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, SpriteModule);

    ECS_COMPONENT_DEFINE(world, Sprite);
}