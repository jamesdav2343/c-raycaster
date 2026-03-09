#include "camera.h"

ECS_COMPONENT_DECLARE(CameraPlane);

void CameraModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, CameraModule);

    ECS_IMPORT(world, TransformModule);

    ECS_COMPONENT_DEFINE(world, CameraPlane);
}