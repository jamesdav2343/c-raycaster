#include "systems/sprite.h"

ECS_SYSTEM_DECLARE(SpriteDraw);

void SpriteSystemsImport(ecs_world_t* world)
{
    ECS_IMPORT(world, SpriteComponents);

    ECS_SYSTEM_DEFINE(world, SpriteDraw, EcsOnUpdate, Sprite);

    ECS_MODULE(world, SpriteSystems);
}

// This must execute after raycaster draw has executed
void SpriteDraw(ecs_iter_t* it)
{
    Sprite* sprite = ecs_field(it, Sprite, 0);

    for (int i = 0; i < it->count; i++) {
        // printf("sprite draw running for %llu -> %d\n", it->entities[i], sprite[i].sprite_id);
    }
}