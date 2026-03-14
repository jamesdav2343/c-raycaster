#ifndef CAMERA_H
#define CAMERA_H

#include <flecs.h>
#include "transform.h"

extern ECS_COMPONENT_DECLARE(Plane);

// Represents the camera plane
typedef Vector2 Plane;

void CameraModuleImport(ecs_world_t *world);

#endif