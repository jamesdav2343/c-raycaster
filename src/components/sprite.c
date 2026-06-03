#include "components/sprite.h"

ECS_COMPONENT_DECLARE(Sprite);

void SpriteComponentsImport(ecs_world_t* world)
{
    ECS_COMPONENT_DEFINE(world, Sprite);
    ECS_MODULE(world, SpriteComponents);
}