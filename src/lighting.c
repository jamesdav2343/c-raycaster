#include "lighting.h"
#include "debug.h"
#include <glib.h>

#define DECAY 0.3f
#define NUM_DIRECTIONS 4
#define NUM_LIGHTS 3
#define INITIAL_INTENSITY 15

Uint8 light_map[ROWS * COLS] = { 0 };

void test(gpointer data, gpointer user_data) { printf("%zu\n", data); }

void bake_light_map()
{
    light_sources = calloc(NUM_LIGHTS, sizeof(size_t));

    light_sources[0] = 44;
    light_sources[1] = 19;
    light_sources[2] = 89;

    // Nodes in graph should be the indices of the light sources
    GQueue* light_queue = g_queue_new();
    Uint8 light_intensities[ROWS * COLS] = { 0 };

    for (int i = 0; i < NUM_LIGHTS; i++) {
        g_queue_push_tail(light_queue, light_sources[i]);
        light_intensities[light_sources[i]] = INITIAL_INTENSITY;
    }

    pretty_print_grid(light_intensities, COLS, 2);

    g_queue_foreach(light_queue, (GFunc)test, NULL);

    free(light_sources);
}