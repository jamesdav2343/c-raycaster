#ifndef CAMERA_H
#define CAMERA_H

#include <flecs.h>
#include "transform.h"

extern ECS_COMPONENT_DECLARE(CameraPlane);

typedef Vector2 CameraPlane;

void CameraModuleImport(ecs_world_t *world);

#endif