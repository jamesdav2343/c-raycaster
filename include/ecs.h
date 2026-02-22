#ifndef ECS_H
#define ECS_H

#include <flecs.h>
#include "transform.h"

ECS_COMPONENT_DECLARE(Vector2);

void rce_init_ecs(ecs_world_t *world);

#endif