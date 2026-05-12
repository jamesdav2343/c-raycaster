
#ifndef INPUT_SYSTEMS_H
#define INPUT_SYSTEMS_H

#include <flecs.h>
#include "transform.h"
#include "components/input.h"

extern ECS_SYSTEM_DECLARE(ControllerUpdate);

void ControllerSystemsImport(ecs_world_t *world);

void ControllerUpdate(ecs_iter_t *it);

#endif