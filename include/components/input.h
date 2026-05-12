#ifndef INPUT_COMPONENTS_H
#define INPUT_COMPONENTS_H

#include "transform.h"
#include <SDL3/SDL.h>
#include <flecs.h>

extern ECS_COMPONENT_DECLARE(Controller);

void ControllerComponentsImport(ecs_world_t* world);

typedef struct Controller {
    int x;
    int y;
} Controller;

#endif