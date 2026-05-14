#ifndef TRANSFORM_COMPONENTS_H
#define TRANSFORM_COMPONENTS_H

#include "types.h"
#include <flecs.h>

extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Direction);

// A position vector in a 2D plane.
typedef Vector2 Position;

// A direction vector in a 2D plane.
typedef Vector2 Direction;

void TransformComponentsImport(ecs_world_t* world);

#endif