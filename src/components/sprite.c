#include "components/sprite.h"

ECS_COMPONENT_DECLARE(Sprite);
ECS_TAG_DECLARE(Drawable);

void SpriteComponentsImport(ecs_world_t* world)
{
    ECS_MODULE(world, SpriteComponents);

    ECS_TAG_DEFINE(world, Drawable);
    ECS_COMPONENT_DEFINE(world, Sprite);
}