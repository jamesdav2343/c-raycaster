#include "systems/sprite.h"
#include "components/game_manager.h"
#include "systems/raycaster.h"

#define SPRITE_COUNT 19

ECS_SYSTEM_DECLARE(SpriteDraw);

ecs_entity_t RaycasterSprite;

static int sprite_order[SPRITE_COUNT];
static double sprite_distance[SPRITE_COUNT];

void SpriteSystemsImport(ecs_world_t* world)
{
    ECS_IMPORT(world, SpriteComponents);
    ECS_IMPORT(world, TransformComponents);

    RaycasterSprite = ecs_new_w_id(world, EcsPhase);

    ecs_add_pair(world, RaycasterSprite, EcsDependsOn, RaycasterMap);

    ecs_system(world,
        { .entity = ecs_entity(world, { .name = "SpriteDraw", .add = ecs_ids(ecs_dependson(RaycasterSprite)) }),
            .query.terms = { { ecs_id(Sprite) }, { ecs_id(Position) } },
            .callback = SpriteDraw });

    // ECS_SYSTEM_DEFINE(world, SpriteDraw, EcsOnUpdate, Sprite);

    ECS_MODULE(world, SpriteSystems);
}

// This system must depend on raycaster draw which handles walls and ceilings
void SpriteDraw(ecs_iter_t* it)
{
    printf("sprite draw executing\n\n");

    Sprite* s = ecs_field(it, Sprite, 0);
    Position* p = ecs_field(it, Position, 1);

    ecs_entity_t player = ecs_lookup(it->world, PLAYER_ENTITY_NAME);

    const Position* position = ecs_get(it->world, player, Position);
    const Direction* direction = ecs_get(it->world, player, Direction);
    const Plane* plane = ecs_get(it->world, player, Plane);
    double* z_buffer = ecs_singleton_get(it->world, ZBuffer)->buffer;

    ht* texture_map = ecs_singleton_get(it->world, Textures)->table;

    int screen_width = 1920;
    int screen_height = 1080;

    PixelBuffer* buffer_data = ecs_singleton_get_mut(it->world, PixelBuffer);

    // Sorts the sprites from far to close
    for (int i = 0; i < it->count; i++) {
        sprite_order[i] = i;
        sprite_distance[i]
            = ((position->x - p[i].x) * (position->x - p[i].x) + (position->y - p[i].y) * (position->y - p[i].y));
    }

    sort_sprites(sprite_order, sprite_distance, it->count);

    for (int i = 0; i < it->count; i++) {
        printf("sprite draw running for %llu -> %f\n", it->entities[i], p[i].x);

        ecs_entity_t entity = it->entities[sprite_order[i]];
        p = ecs_get_mut(it->world, entity, Position);
        s = ecs_get_mut(it->world, entity, Sprite);

        double sprite_x = p->x - position->x;
        double sprite_y = p->y - position->y;

        double inv_det
            = 1.0 / (plane->x * direction->y - direction->x * plane->y); // required for correct matrix multiplication

        double transform_x = inv_det * (direction->y * sprite_x - direction->x * sprite_y);
        double transform_y = inv_det * (-plane->y * sprite_x + plane->x * sprite_y);

        int sprite_screen_x = (int)((screen_width / 2) * (1 + transform_x / transform_y));

        int sprite_height = abs((int)(screen_height / (transform_y)));

        int draw_start_y = -sprite_height / 2 + screen_height / 2;
        if (draw_start_y < 0)
            draw_start_y = 0;

        int draw_end_y = sprite_height / 2 + screen_height / 2;
        if (draw_end_y >= screen_height)
            draw_end_y = screen_height - 1;

        int sprite_width = abs((int)(screen_height / (transform_y)));
        int draw_start_x = -sprite_width / 2 + sprite_screen_x;
        if (draw_start_x < 0)
            draw_start_x = 0;

        int draw_end_x = sprite_width / 2 + sprite_screen_x;
        if (draw_end_x >= screen_width)
            draw_end_x = screen_width - 1;

        // texture stuff
        ht* sprites = (ht*)ht_get(texture_map, "sprites");

        if (sprites == NULL) {
            printf("sprites is null\n");
            return;
        }

        char buffer[1024];
        SDL_itoa(s->sprite_id, buffer, 10);

        SDL_Surface* surface = (SDL_Surface*)ht_get(sprites, buffer);

        if (surface == NULL) {
            printf("could not get surface\n");
            return;
        }

        Uint32* pixels = (Uint32*)surface->pixels;

        for (int stripe = draw_start_x; stripe < draw_end_x; stripe++) {
            int tex_x
                = (int)(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) * SPRITE_TEXTURE_WIDTH / sprite_width)
                / 256;

            if (transform_y > 0 && stripe > 0 && stripe < screen_width && transform_y < z_buffer[stripe]) {

                for (int y = draw_start_y; y < draw_end_y; y++) // for every pixel of the current stripe
                {
                    // 256 and 128 factors to avoid floats
                    int d = (y) * 256 - screen_height * 128 + sprite_height * 128;

                    int tex_y = ((d * SPRITE_TEXTURE_HEIGHT) / sprite_height) / 256;

                    // get current color from the texture
                    // Uint32 color = textures[sprite[sprite_order[i]].texture][SPRITE_TEXTURE_WIDTH * tex_y +
                    // tex_x];
                    Uint32 color = pixels[SPRITE_TEXTURE_WIDTH * tex_y + tex_x];
                    // Uint32 color = WHITE;

                    if ((color & 0x00FFFFFF) != 0) {
                        // paint pixel if it isn't black, black is the invisible color
                        buffer_data->pixels[stripe + (y * buffer_data->width)] = color;
                    }
                }
            }
        }
    }
}