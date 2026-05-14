#ifndef INPUT_SYSTEMS_H
#define INPUT_SYSTEMS_H

#include "components/input.h"
#include <flecs.h>

extern ECS_SYSTEM_DECLARE(ControllerUpdate);

void InputSystemsImport(ecs_world_t* world);

void ControllerUpdate(ecs_iter_t* it);

#endif