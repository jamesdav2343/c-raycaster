#ifndef CAMERA_H
#define CAMERA_H

#include <flecs.h>
#include "types.h"
#include "components/transform.h"

extern ECS_COMPONENT_DECLARE(Plane);

// The camera plane.
typedef Vector2 Plane;

void CameraModuleImport(ecs_world_t* world);

#endif