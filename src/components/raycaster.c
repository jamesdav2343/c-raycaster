#include "components/raycaster.h"

ECS_TAG_DECLARE(Raycaster);
ECS_COMPONENT_DECLARE(BufferData);

void RaycasterComponentsImport(ecs_world_t* world)
{
    ECS_TAG_DEFINE(world, Raycaster);
    ECS_COMPONENT_DEFINE(world, BufferData);

    ECS_MODULE(world, RaycasterComponents);

    printf("\n\n\n raycaster components imported\n\n\n");
}