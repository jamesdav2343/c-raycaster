#include "components/raycaster.h"

ECS_TAG_DECLARE(Raycaster);
ECS_COMPONENT_DECLARE(PixelBuffer);

void RaycasterComponentsImport(ecs_world_t* world)
{
    ECS_TAG_DEFINE(world, Raycaster);
    ECS_COMPONENT_DEFINE(world, PixelBuffer);

    ECS_MODULE(world, RaycasterComponents);

    printf("\n\n\n raycaster components imported\n\n\n");
}