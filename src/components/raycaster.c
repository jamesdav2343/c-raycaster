#include "components/raycaster.h"

ECS_TAG_DECLARE(Raycaster);
ECS_COMPONENT_DECLARE(PixelBuffer);
ECS_COMPONENT_DECLARE(ZBuffer);
ECS_COMPONENT_DECLARE(Sprite);
ECS_COMPONENT_DECLARE(ScreenTexture);

void RaycasterComponentsImport(ecs_world_t* world)
{
    ECS_TAG_DEFINE(world, Raycaster);
    ECS_COMPONENT_DEFINE(world, PixelBuffer);
    ECS_COMPONENT_DEFINE(world, ZBuffer);
    ECS_COMPONENT_DEFINE(world, Sprite);
    ECS_COMPONENT_DEFINE(world, ScreenTexture);

    ECS_MODULE(world, RaycasterComponents);
}