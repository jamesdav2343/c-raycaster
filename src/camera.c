#include "camera.h"

ECS_COMPONENT_DECLARE(Plane);

void CameraModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, CameraModule);

    ECS_IMPORT(world, TransformComponents);

    ECS_COMPONENT_DEFINE(world, Plane);
}