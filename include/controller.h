#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL3/SDL.h>
#include <flecs.h>
#include "transform.h"

extern ECS_COMPONENT_DECLARE(Controller);
extern ECS_SYSTEM_DECLARE(ControllerUpdate);

typedef struct Controller
{
    int x;
    int y;
} Controller;

void ControllerModuleImport(ecs_world_t *world);

void ControllerUpdate(ecs_iter_t *it);

Vector2 get_input_direction();

#endif