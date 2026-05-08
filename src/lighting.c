#include "lighting.h"
#include <glib.h>

#define DECAY 0.3f
#define NUM_DIRECTIONS 4

Uint8 light_map[ROWS * COLS] = { 0 };

void bake_light_map()
{
    // Nodes in graph should be the indices of the light sources
    GQueue* deque = g_queue_new();

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            printf("%d ", light_map[x + (y * COLS)]);
        }
        printf("\n");
    }
}